// CasServer.cpp: CCasServer �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "CasServer.h"
#include "BCasCard.h"


CCasServer::CCasServer(ICasServerHandler *pEventHandler)
	: m_pEventHandler(pEventHandler)
	, m_hServerThread(NULL)
{

}

CCasServer::~CCasServer(void)
{
	CloseServer();
}

const BOOL CCasServer::OpenServer(const WORD wServerPort)
{
	// �J�[�h���[�_���݃`�F�b�N
	CBcasCard BCasCard;
	if(!BCasCard.OpenCard())return FALSE;

	// �T�[�o�\�P�b�g�I�[�v��
	if(!m_pSocket.Listen(wServerPort))return FALSE;

	// �T�[�o�X���b�h�N��
	DWORD dwThreadID = 0UL;
	if(!(m_hServerThread = ::CreateThread(NULL, 0UL, CCasServer::ServerThreadRaw, (LPVOID)this, 0UL, &dwThreadID))){
		m_pSocket.Close();
		return FALSE;
		}

	return TRUE;
}

void CCasServer::CloseServer(void)
{
	// �T�[�o�\�P�b�g�N���[�Y
	m_pSocket.Close();

	// �X���b�h�n���h���J��
	if(m_hServerThread){
		::CloseHandle(m_hServerThread);
		m_hServerThread = NULL;
		}

	// �S�N���C�A���g�ؒf
	m_Lock.Lock();
	
	for(ClientList::iterator It = m_ClientList.begin() ; It != m_ClientList.end() ; It++){
		It->first->CloseClient();
		}

	m_Lock.Unlock();
	
	// �S�N���C�A���g�̏I����҂�
	while(m_ClientList.size())::Sleep(1UL);
}

const DWORD CCasServer::GetClientNum(void) const
{
	// �ڑ����̃N���C�A���g����Ԃ�
	return m_ClientList.size();
}

void CCasServer::OnCasClientEvent(CCasClient *pClient, const DWORD dwEventID, PVOID pParam)
{
	CBlockLock AutoLock(&m_Lock);

	// �N���C�A���g�C�x���g
	switch(dwEventID){
		case CCasClient::EID_CONNECTED :
			// ���X�g�ɒǉ�
			m_ClientList[pClient] = pClient;
			m_pEventHandler->OnCasServerEvent(this, CSEI_CONNECTED);
			break;
			
		case CCasClient::EID_DISCONNECTED :
			// ���X�g����폜
			m_ClientList.erase(pClient);
			m_pEventHandler->OnCasServerEvent(this, CSEI_DISCONNECTED);
			break;
		}
}

void CCasServer::ServerThread(void)
{
	// �A�N�Z�v�g���[�v
	CSmartSock *pNewSocket;
	
	while(pNewSocket = m_pSocket.Accept()){
		// �N���C�A���g�C���X�^���X����
		new CCasClient(this, pNewSocket);
		}
}

DWORD WINAPI CCasServer::ServerThreadRaw(LPVOID pParam)
{
	// �T�[�o�X���b�h
	static_cast<CCasServer *>(pParam)->ServerThread();

	return 0UL;
}
