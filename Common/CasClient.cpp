// CasClient.cpp: CCasClient クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CasClient.h"


#define TCP_TIMEOUT		1000UL		// 1秒


CCasClient::CCasClient(ICasClientHandler *pEventHandler, CSmartSock *pSocket)
	: CBcasCard()
	, m_pSocket(pSocket)
	, m_pEventHandler(pEventHandler)
	, m_hClientThread(NULL)
{
	// クライアントスレッド起動
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
	// クライアントを切断する
	m_pSocket->Close();
}

void CCasClient::ClientThread(void)
{
	// カードリーダを開く
	if(!OpenCard())return;
	
	// 受信バッファ
	BYTE byDataLen;
	BYTE RecvBuf[256];
	DWORD dwRecvSize;
	
	// メッセージ受信ループ
	while(1){
		// ヘッダを受信
		if(!m_pSocket->Recv(&byDataLen, 1UL, TCP_TIMEOUT)){
			if(m_pSocket->GetLastError() == CSmartSock::EC_TIMEOUT)continue;
			else break;			
			}

		// データを受信
		if(!m_pSocket->Recv(RecvBuf, byDataLen, TCP_TIMEOUT))break;
	
		// カード送受信
		if(!TransmitCommand(RecvBuf, byDataLen, &RecvBuf[1], sizeof(RecvBuf) - 1UL, &dwRecvSize))dwRecvSize = 0UL;

		// データを送信
		RecvBuf[0] = (BYTE)dwRecvSize;
		if(!m_pSocket->Send(RecvBuf, dwRecvSize + 1UL, TCP_TIMEOUT))break;
		}

	// カードリーダを閉じる
	CloseCard();
}

DWORD WINAPI CCasClient::ClientThreadRaw(LPVOID pParam)
{
	// クライアントスレッド
	CCasClient *pThis = static_cast<CCasClient *>(pParam);

	// 接続イベント通知
	pThis->SendEvent(EID_CONNECTED);
	
	// メンバ関数にリダイレクトする
	pThis->ClientThread();
	
	// 切断イベント通知
	pThis->SendEvent(EID_DISCONNECTED);
	
	delete pThis;
	
	return 0UL;
}

void CCasClient::SendEvent(const DWORD dwEventID, PVOID pParam)
{
	// ハンドラにイベントを通知する
	if(m_pEventHandler)m_pEventHandler->OnCasClientEvent(this, dwEventID, pParam);
}
