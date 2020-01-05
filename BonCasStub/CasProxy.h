// CasProxy.h: CCasProxy クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////


#pragma once


#include "SmartSock.h"


class CCasProxy
{
public:
	enum
	{
		CPEI_ENTERPROCESS,		// プロセス開始通知
		CPEI_EXITPROCESS,		// プロセス終了通知
		CPEI_GETSERVERIP,		// サーバIP取得
		CPEI_GETSERVERPORT,		// サーバポート取得
		CPEI_CONNECTSUCCESS,	// 接続完了
		CPEI_CONNECTFAILED,		// 接続失敗
		CPEI_DISCONNECTED		// 接続切断
	};
	
	CCasProxy(const HWND hProxyHwnd);
	~CCasProxy(void);

	const BOOL Connect(void);
	const DWORD TransmitCommand(const BYTE *pSendData, const DWORD dwSendSize, BYTE *pRecvData);

	static void SendEnterProcessEvent(const HWND hProxyHwnd);
	static void SendExitProcessEvent(const HWND hProxyHwnd);

protected:
	const DWORD SendProxyEvent(WPARAM wParam) const;

	const HWND m_hProxyHwnd;
	CSmartSock m_Socket;
	
	static DWORD dwErrorDelayTime;
};
