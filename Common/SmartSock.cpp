// SmartSock.cpp: CSmartSock クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <StdLib.h>
#include <Locale.h>
#include "SmartSock.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma warning(disable: 4101) // warning C4101: "ローカル変数は 1 度も使われていません。"
#pragma warning(disable: 4996) // warning C4996: "This function or variable may be unsafe. Consider using _wsplitpath_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details"


#pragma comment(lib, "WS2_32")


//////////////////////////////////////////////////////////////////////
// マクロ定義
//////////////////////////////////////////////////////////////////////

#define CHECK_FREESOCK(R)	if(m_Socket != INVALID_SOCKET){m_dwLastError = EC_SOCKINVALID; return (R);}
#define CHECK_TCPSOCK(R)	if((m_Socket == INVALID_SOCKET) || (m_bSockType != SOCKTYPE_TCP)){m_dwLastError = EC_SOCKINVALID; return (R);}
#define CHECK_UDPSOCK(R)	if((m_Socket == INVALID_SOCKET) || (m_bSockType != SOCKTYPE_UDP)){m_dwLastError = EC_SOCKINVALID; return (R);}


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

DWORD CSmartSock::dwInstanceNum = 0UL;

CSmartSock::CSmartSock()
	: m_Socket(INVALID_SOCKET)
	, m_bSockType(SOCKTYPE_NON)
	, m_dwLastError(EC_NOERROR)
{
	// WinSock2初期化
	if(!(dwInstanceNum++))InitWinSock2();
}

CSmartSock::~CSmartSock()
{
	Close();

	// WinSock2開放
	if(!(--dwInstanceNum))FreeWinSock2();
}

const BOOL CSmartSock::Connect(LPCTSTR lpszHost, const WORD wPort, const DWORD dwTimeOut)
{
	CHECK_FREESOCK(FALSE);

	if(!lpszHost){
		m_dwLastError = EC_PARAMINVALID;
		return FALSE;
		}
		
	// アドレス名からIPアドレス取得
	const DWORD dwIP = HostToIP(lpszHost);

	if(dwIP == INADDR_NONE){
		m_dwLastError = EC_SOCKERROR;
		return FALSE;		
		}
	
	return Connect(dwIP, wPort, dwTimeOut);
}

const BOOL CSmartSock::Connect(const DWORD dwIP, const WORD wPort, const DWORD dwTimeOut)
{
	CHECK_FREESOCK(FALSE);

	// ソケット作成
	if((m_Socket = ::socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		m_dwLastError = EC_SOCKERROR;
		return FALSE;		
		}

	// アドレス設定
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = ::htons(wPort);
	SockAddr.sin_addr.S_un.S_addr = ::htonl(dwIP);

	// 同期コネクト
	if(!dwTimeOut){
		if(::connect(m_Socket, (PSOCKADDR)&SockAddr, sizeof(sockaddr))){
			Close();
			m_dwLastError = EC_SOCKERROR;
			return FALSE;		
			}
	
		m_dwLastError = EC_NOERROR;
		return TRUE;
		}

	// 非同期コネクト
	u_long nArg = 1UL;
	fd_set FdSet;
	struct timeval TimeVal;
	TimeVal.tv_sec = 0UL;
	TimeVal.tv_usec = dwTimeOut * 1000UL;
	FD_ZERO(&FdSet);
	FD_SET(m_Socket, &FdSet);

	try{
		// 非同期に切り替え
		if(::ioctlsocket(m_Socket, FIONBIO, &nArg) == SOCKET_ERROR)throw (const DWORD)__LINE__;

		// コネクト
		if(::connect(m_Socket, (PSOCKADDR)&SockAddr, sizeof(sockaddr)) != SOCKET_ERROR)throw (const DWORD)__LINE__;

		if(::WSAGetLastError() != WSAEWOULDBLOCK)throw (const DWORD)__LINE__;

		// 接続完了待ち
		if(!::select(32, NULL, &FdSet, NULL, &TimeVal))throw (const DWORD)__LINE__;

		// タイムアウト判定
		if(!FD_ISSET(m_Socket, &FdSet)){
			Close();
			m_dwLastError = EC_TIMEOUT;
			return FALSE;
			}

		// 同期に切り替え
		nArg = 0UL;
		if(::ioctlsocket(m_Socket, FIONBIO, &nArg) == SOCKET_ERROR)throw (const DWORD)__LINE__;
		}
	catch(const DWORD dwLine){
		// エラー発生
		Close();
		m_dwLastError = EC_SOCKERROR;
		return FALSE;
		}

	m_bSockType = SOCKTYPE_TCP;
	m_dwLastError = EC_NOERROR;

	return TRUE;
}

const BOOL CSmartSock::Listen(const WORD wPort)
{
	CHECK_FREESOCK(FALSE);

	// ソケット作成
	if((m_Socket = ::socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET){
		m_dwLastError = EC_SOCKERROR;
		return FALSE;		
		}

	// アドレス設定
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = ::htons(wPort);
	SockAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	try{
		// バインド
		if(::bind(m_Socket, (PSOCKADDR)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR)throw (const DWORD)__LINE__;

		// 接続受け入れ
		if(::listen(m_Socket, 5) == SOCKET_ERROR)throw (const DWORD)__LINE__;
		}
	catch(const DWORD dwLine){
		// エラー発生
		Close();
		m_dwLastError = EC_SOCKERROR;
		return FALSE;
		}

	m_bSockType = SOCKTYPE_TCP;
	m_dwLastError = EC_NOERROR;

	return TRUE;
}

CSmartSock * CSmartSock::Accept(void)
{
	CHECK_TCPSOCK(FALSE);
	
	SOCKADDR_IN AddrIn;
	::ZeroMemory(&AddrIn, sizeof(AddrIn));
	int iAddrLen = sizeof(AddrIn);

	// コネクト受け入れ
	SOCKET SockIn = ::accept(m_Socket, (sockaddr *)&AddrIn, &iAddrLen);

	if(SockIn == INVALID_SOCKET){
		Close();
		m_dwLastError = EC_SOCKERROR;
		return NULL;
		}

	CSmartSock *pNewSock = new CSmartSock;
	pNewSock->m_Socket = SockIn;
	pNewSock->m_bSockType = SOCKTYPE_TCP;

	m_dwLastError = EC_NOERROR;

	return pNewSock;
}

const BOOL CSmartSock::Send(const BYTE *pBuff, const DWORD dwLen, const DWORD dwTimeOut)
{
	CHECK_TCPSOCK(FALSE);

	if(!pBuff || !dwLen){
		m_dwLastError = EC_PARAMINVALID;
		return FALSE;
		}

	// 指定サイズ送信
	DWORD dwRef = 0UL, dwSend = 0UL;

	do{
		if(!(dwRef = SendOnce(pBuff + dwSend, dwLen - dwSend, dwTimeOut)))return FALSE;
		}
	while((dwSend += dwRef) < dwLen);

	return TRUE;
}

const BOOL CSmartSock::Recv(BYTE *pBuff, const DWORD dwLen, const DWORD dwTimeOut)
{
	CHECK_TCPSOCK(FALSE);

	if(!pBuff || !dwLen){
		m_dwLastError = EC_PARAMINVALID;
		return FALSE;
		}

	// 指定サイズ受信
	DWORD dwRef = 0UL, dwRecv = 0UL;

	do{
		if(!(dwRef = RecvOnce(pBuff + dwRecv, dwLen - dwRecv, dwTimeOut)))return FALSE;
		}
	while((dwRecv += dwRef) < dwLen);

	return TRUE;
}

const DWORD CSmartSock::SendOnce(const BYTE *pBuff, const DWORD dwMaxLen, const DWORD dwTimeOut)
{
	CHECK_TCPSOCK(0UL);

	if(!pBuff || !dwMaxLen){
		m_dwLastError = EC_PARAMINVALID;
		return FALSE;
		}
	
	// タイムアウト設定
	int iValue  = 0, iSize = sizeof(int);

	if(::getsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&iValue, &iSize)){
		Close();
		m_dwLastError = EC_SOCKERROR;
		return 0UL;
		}
	
	if(iValue != (int)dwTimeOut){
		if(::setsockopt(m_Socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&iValue, sizeof(int))){
			Close();
			m_dwLastError = EC_SOCKERROR;
			return 0UL;
			}
		}
	
	// 送信
	const int iRef = ::send(m_Socket, (const char *)pBuff, dwMaxLen, 0);

	if((iRef == SOCKET_ERROR) || !iRef){
		if(::WSAGetLastError() == WSAETIMEDOUT){
			m_dwLastError = EC_TIMEOUT;
			return 0UL;
			}
		else{
			Close();
			m_dwLastError = EC_SOCKERROR;
			return 0UL;
			}
		}
		
	m_dwLastError = EC_NOERROR;
		
	return (DWORD)iRef;
}

const DWORD CSmartSock::RecvOnce(BYTE *pBuff, const DWORD dwMaxLen, const DWORD dwTimeOut)
{
	CHECK_TCPSOCK(0UL);
	
	if(!pBuff || !dwMaxLen){
		m_dwLastError = EC_PARAMINVALID;
		return FALSE;
		}

	// タイムアウト値設定
	int iValue  = 0, iSize = sizeof(int);

	if(::getsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&iValue, &iSize)){
		Close();
		m_dwLastError = EC_SOCKERROR;
		return 0UL;
		}

	if(iValue != (int)dwTimeOut){
		if(::setsockopt(m_Socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&iValue, sizeof(int))){
			Close();
			m_dwLastError = EC_SOCKERROR;
			return 0UL;
			}
		}
	
	// 受信
	const int iRef = ::recv(m_Socket, (char *)pBuff, dwMaxLen, 0);

	if((iRef == SOCKET_ERROR) || !iRef){
		if(::WSAGetLastError() == WSAETIMEDOUT){
			m_dwLastError = EC_TIMEOUT;
			return 0UL;
			}
		else{
			Close();
			m_dwLastError = EC_SOCKERROR;
			return 0UL;
			}
		}
		
	m_dwLastError = EC_NOERROR;
		
	return (DWORD)iRef;
}

const BOOL CSmartSock::GetLocalAddr(DWORD *pdwIP, WORD *pwPort)
{
	CHECK_TCPSOCK(FALSE);

	struct sockaddr_in LocalAddr;
	int AddrLen = sizeof(LocalAddr);
	
	// ローカルアドレス取得
	if(::getsockname(m_Socket, (struct sockaddr *)&LocalAddr, &AddrLen) == SOCKET_ERROR){
		m_dwLastError = EC_SOCKERROR;
		return FALSE;
		}

	if(pdwIP)*pdwIP = ::htonl(LocalAddr.sin_addr.S_un.S_addr);
	if(pwPort)*pwPort = ::ntohs(LocalAddr.sin_port);

	m_dwLastError = EC_NOERROR;

	return TRUE;
}

const BOOL CSmartSock::GetPeerAddr(DWORD *pIP, WORD *pPort)
{
	CHECK_TCPSOCK(FALSE);

	struct sockaddr_in PeerAddr;
	int AddrLen = sizeof(PeerAddr);
	
	// ピアアドレス取得
	if(::getpeername(m_Socket, (struct sockaddr *)&PeerAddr, &AddrLen) == SOCKET_ERROR){
		m_dwLastError = EC_SOCKERROR;
		return FALSE;
		}

	if(pIP)*pIP = ::htonl(PeerAddr.sin_addr.S_un.S_addr);
	if(pPort)*pPort = ::ntohs(PeerAddr.sin_port);

	m_dwLastError = EC_NOERROR;

	return TRUE;
}

const BOOL CSmartSock::Bind()
{
	CHECK_FREESOCK(FALSE);

	// UDPソケット作成
	if((m_Socket = ::socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		m_dwLastError = EC_SOCKERROR;
		return FALSE;
		}

	// アドレス設定
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = 0U;
	SockAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// バインド
	if(::bind(m_Socket, (struct sockaddr *)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR){
		Close();
		m_dwLastError = EC_SOCKERROR;
		return FALSE;
		}

	m_dwLastError = EC_NOERROR;
	m_bSockType = SOCKTYPE_UDP;
	
	return TRUE;
}

const DWORD CSmartSock::SendTo(const DWORD dwIP, const WORD wPort, const BYTE *pBuff, const DWORD dwLen)
{
	CHECK_UDPSOCK(0UL);

	if(!pBuff || !dwLen){
		m_dwLastError = EC_PARAMINVALID;
		return 0UL;
		}

	// アドレス設定
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = ::htons(wPort);
	SockAddr.sin_addr.S_un.S_addr = ::htonl(dwIP);

	// 送信
	int iSend = sendto(m_Socket, (const char *)pBuff, dwLen, 0, (struct sockaddr *)&SockAddr, sizeof(SockAddr));
	
	if(iSend == SOCKET_ERROR){
		m_dwLastError = EC_SOCKERROR;
		return 0UL;
		}

	m_dwLastError = EC_NOERROR;

	return (DWORD)iSend;
}

const DWORD CSmartSock::SendTo(LPCTSTR lpszHost, const WORD wPort, const BYTE *pBuff, const DWORD dwLen)
{
	CHECK_UDPSOCK(0UL);

	if(!lpszHost || !pBuff || !dwLen){
		m_dwLastError = EC_PARAMINVALID;
		return 0UL;
		}
		
	// アドレス名からIPアドレス取得
	const DWORD dwIP = HostToIP(lpszHost);

	if(dwIP == INADDR_NONE){
		m_dwLastError = EC_SOCKERROR;
		return 0UL;		
		}
	
	return SendTo(dwIP, wPort, pBuff, dwLen);
}

const DWORD CSmartSock::RecvFrom(BYTE *pBuff, const DWORD dwLen, DWORD *pdwIP, WORD *pwPort)
{
	CHECK_UDPSOCK(0UL);

	if(!pBuff || !dwLen){
		m_dwLastError = EC_PARAMINVALID;
		return 0UL;
		}

	// アドレス設定
	int iSockSize = sizeof(SOCKADDR_IN);
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = 0U;
	SockAddr.sin_addr.S_un.S_addr = 0UL;

	// 受信
	const int iRecv = ::recvfrom(m_Socket, (char *)pBuff, dwLen, 0, (struct sockaddr *)&SockAddr, &iSockSize);

	if(iRecv == SOCKET_ERROR){
		m_dwLastError = EC_SOCKERROR;
		return 0UL;
		}

	if(pdwIP)*pdwIP = SockAddr.sin_addr.S_un.S_addr;
	if(pwPort)*pwPort = ntohs(SockAddr.sin_port);

	m_dwLastError = EC_NOERROR;

	return (DWORD)iRecv;
}

const BOOL CSmartSock::Close()
{
	// ソケットクローズ
	if(m_Socket != INVALID_SOCKET){
		if(m_bSockType = SOCKTYPE_TCP){
			char byData;
			::shutdown(m_Socket, SD_BOTH);
			while(::recv(m_Socket, &byData, 1, 0) == 1);
			}

		::closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		}
	
	m_bSockType = SOCKTYPE_NON;
	m_dwLastError = EC_NOERROR;
	
	return TRUE;
}

const DWORD CSmartSock::HostToIP(LPCTSTR lpszHost)
{
#ifdef _UNICODE
	char szHost[1024] = {"\0"};
	::wcstombs(szHost, lpszHost, sizeof(szHost) - 1);
#else
	LPCSTR szHost = lpszHost;
#endif

	// ホスト名からIPアドレス取得
	const DWORD dwIP = ::inet_addr(szHost);

	if(dwIP == INADDR_NONE){
		struct hostent *pHost = ::gethostbyname(szHost);
		if(!pHost){		
			return INADDR_NONE;
			}
		else return *((DWORD *)pHost->h_addr_list[0]);
		}
	else return ::htonl(dwIP);
}

const DWORD CSmartSock::IPToHost(LPTSTR lpszHost, const DWORD dwIP)
{
	if(!lpszHost)return FALSE;

	// IPアドレスからホスト名取得
	const DWORD dwNetIP = ::htonl(dwIP);
	struct hostent *pHost = ::gethostbyaddr((const char *)&dwNetIP, sizeof(dwNetIP), AF_INET);
	if(!pHost)return FALSE;

#ifdef _UNICODE
	::mbstowcs(lpszHost, pHost->h_name, ::lstrlenA(pHost->h_name));
#else
	::lstrcpy(lpszHost, pHost->h_name);
#endif

	return ::lstrlen(lpszHost);
}

const DWORD CSmartSock::GetLastError() const
{
	// 最後に発生したエラーを返す
	return m_dwLastError;
}

const BOOL CSmartSock::InitWinSock2(void)
{
	WSADATA WsaData;

#ifdef _UNICODE
	::setlocale(LC_ALL, "japanese");
#endif

	// WinSock2初期化
	if(::WSAStartup(MAKEWORD(2, 2), &WsaData))return FALSE;

	if((LOBYTE(WsaData.wVersion) != 2U) || (HIBYTE(WsaData.wVersion) != 2U))return FALSE;

	return TRUE;
}

const BOOL CSmartSock::FreeWinSock2(void)
{
	// WinSock2開放
	return (::WSACleanup())? TRUE : FALSE;
}
