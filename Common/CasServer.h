// CasServer.h: CCasServer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <map>
#include "SmartSock.h"
#include "CasClient.h"
#include "TsUtilClass.h"


using std::map;


class CCasServer : protected CCasClient::ICasClientHandler
{
public:
	class ICasServerHandler
	{
	public:
		virtual void OnCasServerEvent(CCasServer *pCasServer, const WORD wEventID) = 0;
	};

	enum
	{
		CSEI_CONNECTED,		// クライアント接続
		CSEI_DISCONNECTED,	// クライアント切断
	};

	CCasServer(void);
	CCasServer(ICasServerHandler *pEventHandler);
	~CCasServer(void);

	const BOOL OpenServer(const WORD wServerPort);
	void CloseServer(void);
	
	const DWORD GetClientNum(void) const;

protected:
	virtual void OnCasClientEvent(CCasClient *pClient, const DWORD dwEventID, PVOID pParam);

	void ServerThread(void);
	static DWORD WINAPI ServerThreadRaw(LPVOID pParam);

	ICasServerHandler *m_pEventHandler;

	CSmartSock m_pSocket;
	HANDLE m_hServerThread;
	
	typedef map<CCasClient *, CCasClient *> ClientList;
	ClientList m_ClientList;
	
	CCriticalLock m_Lock;
};
