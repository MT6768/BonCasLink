// CasClient.h: CCasClient �N���X�̃C���^�[�t�F�C�X
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
		EID_CONNECTED,		// �N���C�A���g�ڑ�
		EID_DISCONNECTED	// �N���C�A���g�ؒf
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
