// CasClient.h: CCasClient クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "BcasCard.h"
#include "SmartSock.h"


class CCasClient : private CBcasCard
{
public:
	class ICasClientHandler
	{
	public:
		virtual void OnCasClientEvent(CCasClient *pClient, const DWORD dwEventID, PVOID pParam) = 0;
	};

	enum
	{
		EID_CONNECTED,		// クライアント接続
		EID_DISCONNECTED	// クライアント切断
	};

	CCasClient(ICasClientHandler *pEventHandler, CSmartSock *pSocket);
	~CCasClient(void);

	void CloseClient(void);

protected:
	void ClientThread(void);
	static DWORD WINAPI ClientThreadRaw(LPVOID pParam);

	void SendEvent(const DWORD dwEventID, PVOID pParam = NULL);

	CSmartSock *m_pSocket;
	ICasClientHandler *m_pEventHandler;
	HANDLE m_hClientThread;
};
