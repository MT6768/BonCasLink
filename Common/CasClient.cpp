// CasClient.cpp: CCasClient �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CasClient.h"


#define TCP_TIMEOUT		1000UL		// 1�b


CCasClient::CCasClient(ICasClientHandler *pEventHandler, CSmartSock *pSocket)
	: CBcasCard()
	, m_pSocket(pSocket)
	, m_pEventHandler(pEventHandler)
	, m_hClientThread(NULL)
{
	// �N���C�A���g�X���b�h�N��
	DWORD dwThreadID = 0UL;
	if(!(m_hClientThread = ::CreateThread(NULL, 0UL, CCasClient::ClientThreadRaw, (LPVOID)this, 0UL, &dwThreadID))){
		delete this;
		}
}

CCasClient::~CCasClient(void)
{
	delete m_pSocket;
	if(m_hClientThread)::CloseHandle(m_hClientThread);
}

void CCasClient::CloseClient(void)
{
	// �N���C�A���g��ؒf����
	m_pSocket->Close();
}

void CCasClient::ClientThread(void)
{
	// �J�[�h���[�_���J��
	if(!OpenCard())return;
	
	// ��M�o�b�t�@
	BYTE byDataLen;
	BYTE RecvBuf[256];
	DWORD dwRecvSize;
	
	// ���b�Z�[�W��M���[�v
	while(1){
		// �w�b�_����M
		if(!m_pSocket->Recv(&byDataLen, 1UL, TCP_TIMEOUT)){
			if(m_pSocket->GetLastError() == CSmartSock::EC_TIMEOUT)continue;
			else break;			
			}

		// �f�[�^����M
		if(!m_pSocket->Recv(RecvBuf, byDataLen, TCP_TIMEOUT))break;
	
		// �J�[�h����M
		if(!TransmitCommand(RecvBuf, byDataLen, &RecvBuf[1], sizeof(RecvBuf) - 1UL, &dwRecvSize))dwRecvSize = 0UL;

		// �f�[�^�𑗐M
		RecvBuf[0] = (BYTE)dwRecvSize;
		if(!m_pSocket->Send(RecvBuf, dwRecvSize + 1UL, TCP_TIMEOUT))break;
		}

	// �J�[�h���[�_�����
	CloseCard();
}

DWORD WINAPI CCasClient::ClientThreadRaw(LPVOID pParam)
{
	// �N���C�A���g�X���b�h
	CCasClient *pThis = static_cast<CCasClient *>(pParam);

	// �ڑ��C�x���g�ʒm
	pThis->SendEvent(EID_CONNECTED);
	
	// �����o�֐��Ƀ��_�C���N�g����
	pThis->ClientThread();
	
	// �ؒf�C�x���g�ʒm
	pThis->SendEvent(EID_DISCONNECTED);
	
	delete pThis;
	
	return 0UL;
}

void CCasClient::SendEvent(const DWORD dwEventID, PVOID pParam)
{
	// �n���h���ɃC�x���g��ʒm����
	if(m_pEventHandler)m_pEventHandler->OnCasClientEvent(this, dwEventID, pParam);
}
