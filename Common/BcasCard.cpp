// BcasCard.cpp: CBcasCard クラスのインプリメンテーション
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
	// 内部状態初期化
	::ZeroMemory(&m_BcasCardInfo, sizeof(m_BcasCardInfo));
	::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));

	// リソースマネージャコンテキスト確立
	if(::SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &m_ScardContext) != SCARD_S_SUCCESS){
		m_dwLastError = BCEC_NOTESTABLISHED;
		}
	else{
		m_bIsEstablish = true;
		
		// カードリーダ列挙
		EnumCardReader();
		}
}

CBcasCard::~CBcasCard()
{
	CloseCard();

	// リソースマネージャコンテキストの開放
	if(m_bIsEstablish)::SCardReleaseContext(m_ScardContext);
}

const DWORD CBcasCard::GetCardReaderNum(void) const
{
	// カードリーダー数を返す
	return m_CardReaderArray.size();
}

LPCTSTR CBcasCard::GetCardReaderName(const DWORD dwIndex) const
{
	// カードリーダー名を返す
	return (dwIndex < GetCardReaderNum())? m_CardReaderArray[dwIndex].c_str() : NULL;
}

const bool CBcasCard::OpenCard(LPCTSTR lpszReader)
{
	// リソースマネージャコンテキストの確立
	if(!m_bIsEstablish){
		m_dwLastError = BCEC_NOTESTABLISHED;
		return false;
		}
	
	// 一旦クローズする
	CloseCard();


	if(lpszReader){
		// 指定されたカードリーダに対してオープンを試みる
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
		// 全てのカードリーダに対してオープンを試みる
		DWORD dwIndex = 0UL;
	
		while(GetCardReaderName(dwIndex)){
			if(OpenCard(GetCardReaderName(dwIndex++)))return true;			
			}
		
		return false;
		}

	// カード初期化
	if(!InitialSetting())return false;

	m_dwLastError = BCEC_NOERROR;

	return true;
}

void CBcasCard::CloseCard(void)
{
	// カードをクローズする
	if(m_hBcasCard){
		::SCardDisconnect(m_hBcasCard, SCARD_LEAVE_CARD);
		m_hBcasCard = NULL;
		}
}

const DWORD CBcasCard::GetLastError(void) const
{
	// 最後に発生したエラーを返す
	return m_dwLastError;
}

const bool CBcasCard::EnumCardReader(void)
{
	// カードリーダを列挙する
	DWORD dwBuffSize = 0UL;
	
	switch(::SCardListReaders(m_ScardContext, NULL, NULL, &dwBuffSize)){
		case SCARD_E_NO_READERS_AVAILABLE :
			// カードリーダが見つからない
			m_dwLastError = BCEC_NOCARDREADERS;
			return false;

		case SCARD_S_SUCCESS :
			// バッファサイズ取得成功
			break;
		
		default:
			// エラー
			m_dwLastError = BCEC_INTERNALERROR;		
			return false;
		}

	// バッファ確保
	auto_ptr<TCHAR> szReaders(new TCHAR[dwBuffSize]);

	switch(::SCardListReaders(m_ScardContext, NULL, szReaders.get(), &dwBuffSize)){
		case SCARD_E_NO_READERS_AVAILABLE :
			// カードリーダが見つからない
			m_dwLastError = BCEC_NOCARDREADERS;
			return false;

		case SCARD_S_SUCCESS : {
			// カードリーダ名保存
			LPTSTR lpszCurReader = szReaders.get();
			m_CardReaderArray.clear();
			
			while(*lpszCurReader){
				m_CardReaderArray.push_back(lpszCurReader);
				lpszCurReader += m_CardReaderArray.back().length() + 1UL;
				}
			
			break;
			}
		
		default:
			// エラー
			m_dwLastError = BCEC_INTERNALERROR;		
			return false;
		}

	m_dwLastError = BCEC_NOERROR;

	return true;
}

const bool CBcasCard::TransmitCommand(const BYTE *pSendData, const DWORD dwSendSize, BYTE *pRecvData, const DWORD dwMaxRecv, DWORD *pdwRecvSize)
{
	DWORD dwRecvSize = dwMaxRecv;

	// データ送受信
	DWORD dwReturn = ::SCardTransmit(m_hBcasCard, SCARD_PCI_T1, pSendData, dwSendSize, NULL, pRecvData, &dwRecvSize);
	
	// 受信サイズ格納
	if(pdwRecvSize)*pdwRecvSize = dwRecvSize;

	return (dwReturn == SCARD_S_SUCCESS)? true : false;
}

const bool CBcasCard::InitialSetting(void)
{
	static const BYTE InitSettingCmd[] = {0x90U, 0x30U, 0x00U, 0x00U, 0x00U};

	// 「Initial Setting Conditions Command」を処理する
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return false;
		}

	// バッファ準備
	DWORD dwRecvSize = 0UL;
	BYTE RecvData[1024];
	::ZeroMemory(RecvData, sizeof(RecvData));
	
	// コマンド送信
	if(!TransmitCommand(InitSettingCmd, sizeof(InitSettingCmd), RecvData, sizeof(RecvData), &dwRecvSize)){
		m_dwLastError = BCEC_TRANSMITERROR;
		return false;
		}

	if(dwRecvSize < 57UL){
		m_dwLastError = BCEC_TRANSMITERROR;
		return false;		
		}

	// レスポンス解析
	::CopyMemory(m_BcasCardInfo.BcasCardID, &RecvData[8], 6UL);		// +8	Card ID
	::CopyMemory(m_BcasCardInfo.SystemKey, &RecvData[16], 32UL);	// +16	Descrambling system key
	::CopyMemory(m_BcasCardInfo.InitialCbc, &RecvData[48], 8UL);	// +48	Descrambler CBC initial value

	// ECMステータス初期化
	::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));

	return true;
}

const BYTE * CBcasCard::GetBcasCardID(void)
{
	// Card ID を返す
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}
	
	m_dwLastError = BCEC_NOERROR;
	
	return m_BcasCardInfo.BcasCardID;
}

const BYTE * CBcasCard::GetInitialCbc(void)
{
	// Descrambler CBC Initial Value を返す
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}
	
	m_dwLastError = BCEC_NOERROR;
	
	return m_BcasCardInfo.InitialCbc;
}

const BYTE * CBcasCard::GetSystemKey(void)
{
	// Descrambling System Key を返す
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

	// 「ECM Receive Command」を処理する
	if(!m_hBcasCard){
		m_dwLastError = BCEC_CARDNOTOPEN;
		return NULL;
		}

	// ECMサイズをチェック
	if(!pEcmData || (dwEcmSize < 30UL) || (dwEcmSize > 256UL)){
		m_dwLastError = BCEC_BADARGUMENT;
		return NULL;
		}

	// キャッシュをチェックする
	if(!StoreEcmData(pEcmData, dwEcmSize)){
		// ECMが同一の場合はキャッシュ済みKsを返す
		m_dwLastError = BCEC_NOERROR;
		return m_EcmStatus.KsData;
		}

	// バッファ準備
	DWORD dwRecvSize = 0UL;
	BYTE SendData[1024];
	BYTE RecvData[1024];
	::ZeroMemory(RecvData, sizeof(RecvData));

	// コマンド構築
	::CopyMemory(SendData, EcmReceiveCmd, sizeof(EcmReceiveCmd));				// CLA, INS, P1, P2
	SendData[sizeof(EcmReceiveCmd)] = (BYTE)dwEcmSize;							// COMMAND DATA LENGTH
	::CopyMemory(&SendData[sizeof(EcmReceiveCmd) + 1], pEcmData, dwEcmSize);	// ECM
	SendData[sizeof(EcmReceiveCmd) + dwEcmSize + 1] = 0x00U;					// RESPONSE DATA LENGTH

	// コマンド送信
	if(!TransmitCommand(SendData, sizeof(EcmReceiveCmd) + dwEcmSize + 2UL, RecvData, sizeof(RecvData), &dwRecvSize)){
		::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));
		m_dwLastError = BCEC_TRANSMITERROR;
		return NULL;
		}

	// サイズチェック
	if(dwRecvSize != 25UL){
		::ZeroMemory(&m_EcmStatus, sizeof(m_EcmStatus));
		m_dwLastError = BCEC_TRANSMITERROR;
		return NULL;
		}	
	
	// レスポンス解析
	::CopyMemory(m_EcmStatus.KsData, &RecvData[6], sizeof(m_EcmStatus.KsData));

	// リターンコード解析
	switch(((WORD)RecvData[4] << 8) | (WORD)RecvData[5]){
		// Purchased: Viewing
		case 0x0200U :	// Payment-deferred PPV
		case 0x0400U :	// Prepaid PPV
		case 0x0800U :	// Tier
			m_dwLastError = BCEC_NOERROR;
			return m_EcmStatus.KsData;
		
		// 上記以外(視聴不可)
		default :
			m_dwLastError = BCEC_ECMREFUSED;
			return NULL;
		}
}

const bool CBcasCard::StoreEcmData(const BYTE *pEcmData, const DWORD dwEcmSize)
{
	bool bUpdate = false;
	
	// ECMデータ比較
	if(m_EcmStatus.dwLastEcmSize != dwEcmSize){
		// サイズが変化した
		bUpdate = true;
		}
	else{
		// サイズが同じ場合はデータをチェックする
		for(DWORD dwPos = 0UL ; dwPos < dwEcmSize ; dwPos++){
			if(pEcmData[dwPos] != m_EcmStatus.LastEcmData[dwPos]){
				// データが不一致
				bUpdate = true;
				break;
				}			
			}
		}

	// ECMデータを保存する
	if(bUpdate){
		m_EcmStatus.dwLastEcmSize = dwEcmSize;
		::CopyMemory(m_EcmStatus.LastEcmData, pEcmData, dwEcmSize);
		}

	return bUpdate;
}
