// CasServer.cpp: CCasServer クラスのインプリメンテーション
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
	// カードリーダ存在チェック
	CBcasCard BCasCard;
	if(!BCasCard.OpenCard())return FALSE;

	// サーバソケットオープン
	if(!m_pSocket.Listen(wServerPort))return FALSE;

	// サーバスレッド起動
	DWORD dwThreadID = 0UL;
	if(!(m_hServerThread = ::CreateThread(NULL, 0UL, CCasServer::ServerThreadRaw, (LPVOID)this, 0UL, &dwThreadID))){
		m_pSocket.Close();
		return FALSE;
		}

	return TRUE;
}

void CCasServer::CloseServer(void)
{
	// サーバソケットクローズ
	m_pSocket.Close();

	// スレッドハンドル開放
	if(m_hServerThread){
		::CloseHandle(m_hServerThread);
		m_hServerThread = NULL;
		}

	// 全クライアント切断
	m_Lock.Lock();
	
	for(ClientList::iterator It = m_ClientList.begin() ; It != m_ClientList.end() ; It++){
		It->first->CloseClient();
		}

	m_Lock.Unlock();
	
	// 全クライアントの終了を待つ
	while(m_ClientList.size())::Sleep(1UL);
}

const DWORD CCasServer::GetClientNum(void) const
{
	// 接続中のクライアント数を返す
	return m_ClientList.size();
}

void CCasServer::OnCasClientEvent(CCasClient *pClient, const DWORD dwEventID, PVOID pParam)
{
	CBlockLock AutoLock(&m_Lock);

	// クライアントイベント
	switch(dwEventID){
		case CCasClient::EID_CONNECTED :
			// リストに追加
			m_ClientList[pClient] = pClient;
			m_pEventHandler->OnCasServerEvent(this, CSEI_CONNECTED);
			break;
			
		case CCasClient::EID_DISCONNECTED :
			// リストから削除
			m_ClientList.erase(pClient);
			m_pEventHandler->OnCasServerEvent(this, CSEI_DISCONNECTED);
			break;
		}
}

void CCasServer::ServerThread(void)
{
	// アクセプトループ
	CSmartSock *pNewSocket;
	
	while(pNewSocket = m_pSocket.Accept()){
		// クライアントインスタンス生成
		new CCasClient(this, pNewSocket);
		}
}

DWORD WINAPI CCasServer::ServerThreadRaw(LPVOID pParam)
{
	// サーバスレッド
	static_cast<CCasServer *>(pParam)->ServerThread();

	return 0UL;
}
