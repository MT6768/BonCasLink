// CasProxy.cpp: CCasProxy �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "CasProxy.h"


#define WM_BONCASPROXY_EVENT		(WM_USER + 100U)
#define TCP_TIMEOUT					1000UL				// 1�b


DWORD CCasProxy::dwErrorDelayTime = 0UL;


CCasProxy::CCasProxy(const HWND hProxyHwnd)
	: m_hProxyHwnd(hProxyHwnd)
{

}

CCasProxy::~CCasProxy(void)
{
	// �T�[�o����ؒf
	m_Socket.Close();
}

const BOOL CCasProxy::Connect(void)
{
	// �G���[�������̃K�[�h�C���^�[�o��
	if(dwErrorDelayTime){
		if((::GetTickCount() - dwErrorDelayTime) < TCP_TIMEOUT)return FALSE;
		else dwErrorDelayTime = 0UL;
		}

	// �T�[�o�ɐڑ�
	if(m_Socket.Connect(SendProxyEvent(CPEI_GETSERVERIP), (WORD)SendProxyEvent(CPEI_GETSERVERPORT), TCP_TIMEOUT)){
		SendProxyEvent(CPEI_CONNECTSUCCESS);
		return TRUE;
		}
	else{
		dwErrorDelayTime = ::GetTickCount();
		SendProxyEvent(CPEI_CONNECTFAILED);
		return FALSE;
		}
}

const DWORD CCasProxy::TransmitCommand(const BYTE *pSendData, const DWORD dwSendSize, BYTE *pRecvData)
{
	// ���M�f�[�^����
	BYTE SendBuf[256];
	SendBuf[0] = (BYTE)dwSendSize;
	::CopyMemory(&SendBuf[1], pSendData, dwSendSize);

	try{
		// ���N�G�X�g���M
		if(!m_Socket.Send(SendBuf, dwSendSize + 1UL, TCP_TIMEOUT))throw (const DWORD)__LINE__;
	
		// ���X�|���X�w�b�_��M
		if(!m_Socket.Recv(SendBuf, 1UL, TCP_TIMEOUT))throw (const DWORD)__LINE__;

		// ���X�|���X�f�[�^��M
		if(!m_Socket.Recv(pRecvData, SendBuf[0], TCP_TIMEOUT))throw (const DWORD)__LINE__;
		}
	catch(const DWORD dwLine){
		// �ʐM�G���[����
		m_Socket.Close();
		SendProxyEvent(CPEI_DISCONNECTED);
		return 0UL;
		}
		
	return SendBuf[0];
}

void CCasProxy::SendEnterProcessEvent(const HWND hProxyHwnd)
{
	// �v���Z�X�J�n�ʒm���M
	::SendMessage(hProxyHwnd, WM_BONCASPROXY_EVENT, CPEI_ENTERPROCESS, ::GetCurrentProcessId());
}

void CCasProxy::SendExitProcessEvent(const HWND hProxyHwnd)
{
	// �v���Z�X�I���ʒm���M
	::SendMessage(hProxyHwnd, WM_BONCASPROXY_EVENT, CPEI_EXITPROCESS, ::GetCurrentProcessId());
}

const DWORD CCasProxy::SendProxyEvent(WPARAM wParam) const
{
	// �z�X�g�ɃC�x���g�𑗐M����
	return (DWORD)::SendMessage(m_hProxyHwnd, WM_BONCASPROXY_EVENT, wParam, ::GetCurrentProcessId());
}
