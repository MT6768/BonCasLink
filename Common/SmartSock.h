// SmartSock.h: CSmartSock クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include <WinSock2.h>


// WinSock2タイムアウト対応ラッパークラス	http://2sen.dip.jp/friio/
class CSmartSock
{
public:
	enum					// LastErrorコード
	{
		EC_NOERROR,			// 正常終了
		EC_SOCKERROR,		// ソケットエラー
		EC_TIMEOUT,			// タイムアウト
		EC_SOCKINVALID,		// ソケットが無効
		EC_PARAMINVALID		// パラメータが無効
	};

	CSmartSock();
	virtual ~CSmartSock();

// TCPソケット
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

// UDPソケット
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

	SOCKET m_Socket;				// ソケットハンドル
	BYTE m_bSockType;				// ソケットタイプ
	DWORD m_dwLastError;			// 最終エラーコード
	static DWORD dwInstanceNum;		// インスタンスの数
};
