// SmartSock.h: CSmartSock �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <WinSock2.h>


// WinSock2�^�C���A�E�g�Ή����b�p�[�N���X	http://2sen.dip.jp/friio/
class CSmartSock
{
public:
	enum					// LastError�R�[�h
	{
		EC_NOERROR,			// ����I��
		EC_SOCKERROR,		// �\�P�b�g�G���[
		EC_TIMEOUT,			// �^�C���A�E�g
		EC_SOCKINVALID,		// �\�P�b�g������
		EC_PARAMINVALID		// �p�����[�^������
	};

	CSmartSock();
	virtual ~CSmartSock();

// TCP�\�P�b�g
	const BOOL Connect(LPCTSTR lpszHost, const WORD wPort, const DWORD dwTimeOut = 0UL);
	const BOOL Connect(const DWORD dwIP, const WORD wPort, const DWORD dwTimeOut = 0UL);

	const BOOL Listen(const WORD wPort);
	CSmartSock * Accept(void);

	const BOOL Send(const BYTE *pBuff, const DWORD dwLen, const DWORD dwTimeOut = 0UL);
	const BOOL Recv(BYTE *pBuff, const DWORD dwLen, const DWORD dwTimeOut = 0UL);
	const DWORD SendOnce(const BYTE *pBuff, const DWORD dwMaxLen, const DWORD dwTimeOut = 0UL);
	const DWORD RecvOnce(BYTE *pBuff, const DWORD dwMaxLen, const DWORD dwTimeOut = 0UL);

	const BOOL GetLocalAddr(DWORD *pdwIP, WORD *pwPort = NULL);
	const BOOL GetPeerAddr(DWORD *pdwIP, WORD *pwPort = NULL);

// UDP�\�P�b�g
	const BOOL Bind(void);

	const DWORD SendTo(const DWORD dwIP, const WORD wPort, const BYTE *pBuff, const DWORD dwLen);
	const DWORD SendTo(LPCTSTR lpszHost, const WORD wPort, const BYTE *pBuff, const DWORD dwLen);
	const DWORD RecvFrom(BYTE *pBuff, const DWORD dwLen, DWORD *pdwIP = NULL, WORD *pwPort = NULL);

	const BOOL Close(void);

	static const DWORD HostToIP(LPCTSTR lpszHost);
	static const DWORD IPToHost(LPTSTR lpszHost, const DWORD dwIP);

	const DWORD GetLastError(void) const;

protected:
	static const BOOL InitWinSock2(void);
	static const BOOL FreeWinSock2(void);

	enum {SOCKTYPE_NON, SOCKTYPE_TCP, SOCKTYPE_UDP};

	SOCKET m_Socket;				// �\�P�b�g�n���h��
	BYTE m_bSockType;				// �\�P�b�g�^�C�v
	DWORD m_dwLastError;			// �ŏI�G���[�R�[�h
	static DWORD dwInstanceNum;		// �C���X�^���X�̐�
};
