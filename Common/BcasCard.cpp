// BcasCard.cpp: CBcasCard �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Memory>
#include "BcasCard.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#pragma comment(lib, "WinScard.lib")


using std::auto_ptr;


CBcasCard::CBcasCard()
	: m_hBcasCard(NULL)
	, m_bIsEstablish(false)
	, m_dwLastError(BCEC_NOERROR)
{
	// ������ԏ�����
	::ZeroMemory(&m_BcasCardInfo, sizeof(m_BcasCardInfo));
	::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));

	// ���\�[�X�}�l�[�W���R���e�L�X�g�m��
	if(::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ScardContext) != SCARD_S_SUCCESS){
		m_dwLastError = BCEC_NOTESTABLISHED;
		}
	else{
		m_bIsEstablish = true;
		
		// �J�[�h���[�_��
		EnumCardReader();
		}
}

CBcasCard::~CBcasCard()
{
	CloseCard();

	// ���\�[�X�}�l�[�W���R���e�L�X�g�̊J��
	if(m_bIsEstablish)::SCardReleaseContext(m_ScardContext);
}

const DWORD CBcasCard::GetCardReaderNum(void) const
{
	// �J�[�h���[�_�[����Ԃ�
	return m_CardReaderArray.size();
}

LPCTSTR CBcasCard::GetCardReaderName(const DWORD dwIndex) const
{
	// �J�[�h���[�_�[����Ԃ�
	return (dwIndex < GetCardReaderNum())? m_CardReaderArray[dwIndex].c_str() : NULL;
}

const bool CBcasCard::OpenCard(LPCTSTR lpszReader)
{
	// ���\�[�X�}�l�[�W���R���e�L�X�g�̊m��
	if(!m_bIsEstablish){
		m_dwLastError = BCEC_NOTESTABLISHED;
		return false;
		}
	
	// ��U�N���[�Y����
	CloseCard();


	if(lpszReader){
		// �w�肳�ꂽ�J�[�h���[�_�ɑ΂��ăI�[�v�������݂�
		DWORD dwActiveProtocol = SCARD_PROTOCOL_UNDEFINED;
		
		if(::SCardConnect(m_ScardContext, lpszReader, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &m_hBcasCard, &dwActiveProtocol) != SCARD_S_SUCCESS){
			m_dwLastError = BCEC_CARDOPENERROR;
			return false;
			}

		if(dwActiveProtocol != SCARD_PROTOCOL_T1){
			CloseCard();
			m_dwLastError = BCEC_CARDOPENERROR;
			return false;
			}
		}
	else{
		// �S�ẴJ�[�h���[�_�ɑ΂��ăI�[�v�������݂�
		DWORD dwIndex = 0UL;
	
		while(GetCardReaderName(dwIndex)){
			if(OpenCard(GetCardReaderName(dwIndex++)))return true;			
			}
		
		return false;
		}

	// �J�[�h������
	if(!InitialSetting())return false;

	m_dwLastError = BCEC_NOERROR;

	return true;
}

void CBcasCard::CloseCard(void)
{
	// �J�[�h���N���[�Y����
	if(m_hBcasCard){
		::SCardDisconnect(m_hBcasCard, SCARD_LEAVE_CARD);
		m_hBcasCard = NULL;
		}
}

const DWORD CBcasCard::GetLastError(void) const
{
	// �Ō�ɔ��������G���[��Ԃ�
	return m_dwLastError;
}

const bool CBcasCard::EnumCardReader(void)
{
	// �J�[�h���[�_��񋓂���
	DWORD dwBuffSize = 0UL;
	
	switch(::SCardListReaders(m_ScardContext, NULL, NULL, &dwBuffSize)){
		case SCARD_E_NO_READERS_AVAILABLE :
			// �J�[�h���[�_��������Ȃ�
			m_dwLastError = BCEC_NOCARDREADERS;
			return false;

		case SCARD_S_SUCCESS :
			// �o�b�t�@�T�C�Y�擾����
			break;
		
		default:
			// �G���[
			m_dwLastError = BCEC_INTERNALERROR;		
			return false;
		}

	// �o�b�t�@�m��
	auto_ptr<TCHAR> szReaders(new TCHAR[dwBuffSize]);

	switch(::SCardListReaders(m_ScardContext, NULL, szReaders.get(), &dwBuffSize)){
		case SCARD_E_NO_READERS_AVAILABLE :
			// �J�[�h���[�_��������Ȃ�
			m_dwLastError = BCEC_NOCARDREADERS;
			return false;

		case SCARD_S_SUCCESS : {
			// �J�[�h���[�_���ۑ�
			LPTSTR lpszCurReader = szReaders.get();
			m_CardReaderArray.clear();
			
			while(*lpszCurReader){
				m_CardReaderArray.push_back(lpszCurReader);
				lpszCurReader += m_CardReaderArray.back().length() + 1UL;
				}
			
			break;
			}
		
		default:
			// �G���[
			m_dwLastError = BCEC_INTERNALERROR;		
			return false;
		}

	m_dwLastError = BCEC_NOERROR;

	return true;
}

const bool CBcasCard::TransmitCommand(const BYTE *pSendData, const DWORD dwSendSize, BYTE *pRecvData, const DWORD dwMaxRecv, DWORD *pdwRecvSize)
{
	DWORD dwRecvSize = dwMaxRecv;

	// �f�[�^����M
	DWORD dwReturn = ::SCardTransmit(m_hBcasCard, SCARD_PCI_T1, pSendData, dwSendSize, NULL, pRecvData, &dwRecvSize);
	
	// ��M�T�C�Y�i�[
	if(pdwRecvSize)*pdwRecvSize = dwRecvSize;

	return (dwReturn == SCARD_S_SUCCESS)? true : false;
}

const bool CBcasCard::InitialSetting(void)
{
	static const BYTE InitSettingCmd[] = {0x90U, 0x30U, 0x00U, 0x00U, 0x00U};

	// �uInitial Setting Conditions Command�v����������
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return false;
		}

	// �o�b�t�@����
	DWORD dwRecvSize = 0UL;
	BYTE RecvData[1024];
	::ZeroMemory(RecvData, sizeof(RecvData));
	
	// �R�}���h���M
	if(!TransmitCommand(InitSettingCmd, sizeof(InitSettingCmd), RecvData, sizeof(RecvData), &dwRecvSize)){
		m_dwLastError = BCEC_TRANSMITERROR;
		return false;
		}

	if(dwRecvSize < 57UL){
		m_dwLastError = BCEC_TRANSMITERROR;
		return false;		
		}

	// ���X�|���X���
	::CopyMemory(m_BcasCardInfo.BcasCardID, &RecvData[8], 6UL);		// +8	Card ID
	::CopyMemory(m_BcasCardInfo.SystemKey, &RecvData[16], 32UL);	// +16	Descrambling system key
	::CopyMemory(m_BcasCardInfo.InitialCbc, &RecvData[48], 8UL);	// +48	Descrambler CBC initial value

	// ECM�X�e�[�^�X������
	::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));

	return true;
}

const BYTE * CBcasCard::GetBcasCardID(void)
{
	// Card ID ��Ԃ�
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}
	
	m_dwLastError = BCEC_NOERROR;
	
	return m_BcasCardInfo.BcasCardID;
}

const BYTE * CBcasCard::GetInitialCbc(void)
{
	// Descrambler CBC Initial Value ��Ԃ�
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}
	
	m_dwLastError = BCEC_NOERROR;
	
	return m_BcasCardInfo.InitialCbc;
}

const BYTE * CBcasCard::GetSystemKey(void)
{
	// Descrambling System Key ��Ԃ�
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}
	
	m_dwLastError = BCEC_NOERROR;
	
	return m_BcasCardInfo.SystemKey;
}

const BYTE * CBcasCard::GetKsFromEcm(const BYTE *pEcmData, const DWORD dwEcmSize)
{
	static const BYTE EcmReceiveCmd[] = {0x90U, 0x34U, 0x00U, 0x00U};

	// �uECM Receive Command�v����������
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}

	// ECM�T�C�Y���`�F�b�N
	if(!pEcmData || (dwEcmSize < 30UL) || (dwEcmSize > 256UL)){
		m_dwLastError = BCEC_BADARGUMENT;
		return NULL;
		}

	// �L���b�V�����`�F�b�N����
	if(!StoreEcmData(pEcmData, dwEcmSize)){
		// ECM������̏ꍇ�̓L���b�V���ς�Ks��Ԃ�
		m_dwLastError = BCEC_NOERROR;
		return m_EcmStatus.KsData;
		}

	// �o�b�t�@����
	DWORD dwRecvSize = 0UL;
	BYTE SendData[1024];
	BYTE RecvData[1024];
	::ZeroMemory(RecvData, sizeof(RecvData));

	// �R�}���h�\�z
	::CopyMemory(SendData, EcmReceiveCmd, sizeof(EcmReceiveCmd));				// CLA, INS, P1, P2
	SendData[sizeof(EcmReceiveCmd)] = (BYTE)dwEcmSize;							// COMMAND DATA LENGTH
	::CopyMemory(&SendData[sizeof(EcmReceiveCmd) + 1], pEcmData, dwEcmSize);	// ECM
	SendData[sizeof(EcmReceiveCmd) + dwEcmSize + 1] = 0x00U;					// RESPONSE DATA LENGTH

	// �R�}���h���M
	if(!TransmitCommand(SendData, sizeof(EcmReceiveCmd) + dwEcmSize + 2UL, RecvData, sizeof(RecvData), &dwRecvSize)){
		::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));
		m_dwLastError = BCEC_TRANSMITERROR;
		return NULL;
		}

	// �T�C�Y�`�F�b�N
	if(dwRecvSize != 25UL){
		::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));
		m_dwLastError = BCEC_TRANSMITERROR;
		return NULL;
		}	
	
	// ���X�|���X���
	::CopyMemory(m_EcmStatus.KsData, &RecvData[6], sizeof(m_EcmStatus.KsData));

	// ���^�[���R�[�h���
	switch(((WORD)RecvData[4] << 8) | (WORD)RecvData[5]){
		// Purchased: Viewing
		case 0x0200U :	// Payment-deferred PPV
		case 0x0400U :	// Prepaid PPV
		case 0x0800U :	// Tier
			m_dwLastError = BCEC_NOERROR;
			return m_EcmStatus.KsData;
		
		// ��L�ȊO(�����s��)
		default :
			m_dwLastError = BCEC_ECMREFUSED;
			return NULL;
		}
}

const bool CBcasCard::StoreEcmData(const BYTE *pEcmData, const DWORD dwEcmSize)
{
	bool bUpdate = false;
	
	// ECM�f�[�^��r
	if(m_EcmStatus.dwLastEcmSize != dwEcmSize){
		// �T�C�Y���ω�����
		bUpdate = true;
		}
	else{
		// �T�C�Y�������ꍇ�̓f�[�^���`�F�b�N����
		for(DWORD dwPos = 0UL ; dwPos < dwEcmSize ; dwPos++){
			if(pEcmData[dwPos] != m_EcmStatus.LastEcmData[dwPos]){
				// �f�[�^���s��v
				bUpdate = true;
				break;
				}			
			}
		}

	// ECM�f�[�^��ۑ�����
	if(bUpdate){
		m_EcmStatus.dwLastEcmSize = dwEcmSize;
		::CopyMemory(m_EcmStatus.LastEcmData, pEcmData, dwEcmSize);
		}

	return bUpdate;
}
