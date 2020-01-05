// CasProxy.cpp: CCasProxy クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "CasProxy.h"


#define WM_BONCASPROXY_EVENT		(WM_USER + 100U)
#define TCP_TIMEOUT					1000UL				// 1秒


DWORD CCasProxy::dwErrorDelayTime = 0UL;


CCasProxy::CCasProxy(const HWND hProxyHwnd)
	: m_hProxyHwnd(hProxyHwnd)
{

}

CCasProxy::~CCasProxy(void)
{
	// サーバから切断
	m_Socket.Close();
}

const BOOL CCasProxy::Connect(void)
{
	// エラー発生時のガードインターバル
	if(dwErrorDelayTime){
		if((::GetTickCount() - dwErrorDelayTime) < TCP_TIMEOUT)return FALSE;
		else dwErrorDelayTime = 0UL;
		}

	// サーバに接続
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
	// 送信データ準備
	BYTE SendBuf[256];
	SendBuf[0] = (BYTE)dwSendSize;
	::CopyMemory(&SendBuf[1], pSendData, dwSendSize);

	try{
		// リクエスト送信
		if(!m_Socket.Send(SendBuf, dwSendSize + 1UL, TCP_TIMEOUT))throw (const DWORD)__LINE__;
	
		// レスポンスヘッダ受信
		if(!m_Socket.Recv(SendBuf, 1UL, TCP_TIMEOUT))throw (const DWORD)__LINE__;

		// レスポンスデータ受信
		if(!m_Socket.Recv(pRecvData, SendBuf[0], TCP_TIMEOUT))throw (const DWORD)__LINE__;
		}
	catch(const DWORD dwLine){
		// 通信エラー発生
		m_Socket.Close();
		SendProxyEvent(CPEI_DISCONNECTED);
		return 0UL;
		}
		
	return SendBuf[0];
}

void CCasProxy::SendEnterProcessEvent(const HWND hProxyHwnd)
{
	// プロセス開始通知送信
	::SendMessage(hProxyHwnd, WM_BONCASPROXY_EVENT, CPEI_ENTERPROCESS, ::GetCurrentProcessId());
}

void CCasProxy::SendExitProcessEvent(const HWND hProxyHwnd)
{
	// プロセス終了通知送信
	::SendMessage(hProxyHwnd, WM_BONCASPROXY_EVENT, CPEI_EXITPROCESS, ::GetCurrentProcessId());
}

const DWORD CCasProxy::SendProxyEvent(WPARAM wParam) const
{
	// ホストにイベントを送信する
	return (DWORD)::SendMessage(m_hProxyHwnd, WM_BONCASPROXY_EVENT, wParam, ::GetCurrentProcessId());
}
