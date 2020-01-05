// MllMain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <shlwapi.h>
#include <WinScard.h>
#include "ApiHook.h"
#include "CasProxy.h"


#pragma comment(lib, "shlwapi.lib")


#ifdef _MANAGED
#pragma managed(push, off)
#endif


// 関数型定義
typedef HMODULE (WINAPI *PFNLOADLIBRARYW)(LPCWSTR lpLibFileName);
typedef HMODULE (WINAPI *PFNLOADLIBRARYA)(LPCSTR lpLibFileName);


// プロセス間共有グローバル変数
#pragma data_seg (".HookSection")		
HHOOK hHook = NULL;
HWND hHostHwnd = NULL;
#pragma data_seg ()


// ファイルローカル変数
static HINSTANCE hInstance = NULL;
static PFNLOADLIBRARYW pfnLoadLibraryWOrg = NULL;
static PFNLOADLIBRARYA pfnLoadLibraryAOrg = NULL;
	

// プロトタイプ
static const BOOL IsHookTargetApp(void);
static const BOOL InstallKernelHook(void);
static const BOOL RemoveKernelHook(void);
static const BOOL InstallScardHook(void);
static HANDLE WINAPI LoadLibraryWHook(LPCWSTR lpLibFileName);
static HANDLE WINAPI LoadLibraryAHook(LPCSTR lpLibFileName);


//////////////////////////////////////////////////////////////////////
// DLLエントリー
//////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call){
		case DLL_PROCESS_ATTACH :

#ifdef _DEBUG
		// メモリリーク検出有効
			::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | ::_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)); 
#endif
			hInstance = hModule;
			
			if(::IsHookTargetApp()){
				// WinScard.dll APIフック
				if(::InstallScardHook()){
					// イベント送信
					CCasProxy::SendEnterProcessEvent(hHostHwnd);
					}
				else{
					// Kernel32.dll APIフック
					::InstallKernelHook();
					}
				}

			break;

		case DLL_PROCESS_DETACH :

			// Kernel32.dll APIフック解除
			::RemoveKernelHook();

			// イベント送信
			CCasProxy::SendExitProcessEvent(hHostHwnd);
			break;
		}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// WinScardフック
//////////////////////////////////////////////////////////////////////

static LONG WINAPI SCardConnectAHook(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	// プロキシインスタンス生成
	CCasProxy *pCasProxy = new CCasProxy(hHostHwnd);
	
	// サーバに接続
	if(!pCasProxy->Connect()){
		delete pCasProxy;
		*phCard = NULL;
		return SCARD_E_READER_UNAVAILABLE;
		}

	// ハンドルに埋め込む
	*phCard = reinterpret_cast<SCARDHANDLE>(pCasProxy);
	if(pdwActiveProtocol)*pdwActiveProtocol = SCARD_PROTOCOL_T1;

	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardConnectWHook(SCARDCONTEXT hContext, LPCWSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols, LPSCARDHANDLE phCard, LPDWORD pdwActiveProtocol)
{
	// プロキシインスタンス生成
	CCasProxy *pCasProxy = new CCasProxy(hHostHwnd);

	// サーバに接続
	if(!pCasProxy->Connect()){
		delete pCasProxy;
		*phCard = NULL;
		return SCARD_E_READER_UNAVAILABLE;
		}

	// ハンドルに埋め込む
	*phCard = reinterpret_cast<SCARDHANDLE>(pCasProxy);
	if(pdwActiveProtocol)*pdwActiveProtocol = SCARD_PROTOCOL_T1;

	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardDisconnectHook(SCARDHANDLE hCard, DWORD dwDisposition)
{
	// サーバから切断
	CCasProxy *pCasProxy = reinterpret_cast<CCasProxy *>(hCard);
	if(pCasProxy)delete pCasProxy;

	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardEstablishContextHook(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext)
{
	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardFreeMemoryHook(SCARDCONTEXT hContext, LPCVOID pvMem)
{
	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardListReadersAHook(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders)
{
	static const char szReaderName[] = "BonCasLink Proxy Card Reader\0";

	if(pcchReaders){
		if((*pcchReaders == SCARD_AUTOALLOCATE) && mszReaders){
			*((LPCSTR *)mszReaders) = szReaderName;		
			return SCARD_S_SUCCESS;
			}
		else{
			*pcchReaders = sizeof(szReaderName);
			}
		}

	if(mszReaders)::CopyMemory(mszReaders, szReaderName, sizeof(szReaderName));

	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardListReadersWHook(SCARDCONTEXT hContext, LPCWSTR mszGroups, LPWSTR mszReaders, LPDWORD pcchReaders)
{
	static const WCHAR szReaderName[] = L"BonCasLink Proxy Card Reader\0";

	if(pcchReaders){
		if((*pcchReaders == SCARD_AUTOALLOCATE) && mszReaders){
			*((LPCWSTR *)mszReaders) = szReaderName;		
			return SCARD_S_SUCCESS;
			}
		else{
			*pcchReaders = sizeof(szReaderName);
			}
		}

	if(mszReaders)::CopyMemory(mszReaders, szReaderName, sizeof(szReaderName));

	return SCARD_S_SUCCESS;
}

static LONG WINAPI SCardTransmitHook(SCARDHANDLE hCard, LPCSCARD_IO_REQUEST pioSendPci, LPCBYTE pbSendBuffer, DWORD cbSendLength, LPSCARD_IO_REQUEST pioRecvPci, LPBYTE pbRecvBuffer, LPDWORD pcbRecvLength)
{
	// サーバにリクエスト送受信
	CCasProxy *pCasProxy = reinterpret_cast<CCasProxy *>(hCard);
	if(!pCasProxy)return SCARD_E_READER_UNAVAILABLE;

	const DWORD dwRecvLen = pCasProxy->TransmitCommand(pbSendBuffer, cbSendLength, pbRecvBuffer);
	if(pcbRecvLength)*pcbRecvLength = dwRecvLen;

	return (dwRecvLen)? SCARD_S_SUCCESS : SCARD_E_TIMEOUT;
}

static LONG WINAPI SCardReleaseContextHook(SCARDCONTEXT hContext)
{
	return SCARD_S_SUCCESS;
}

static const BOOL IsHookTargetApp(void)
{
	// アプリケーションのファイル名を取得
	TCHAR szModulePath[_MAX_PATH] = {TEXT('\0')};
	::GetModuleFileName(NULL, szModulePath, sizeof(szModulePath));

	// 下記アプリケーションならばIATを書き換えてはならない
	if(::StrStrI(szModulePath, TEXT("boncasserver.exe"	)) ||
	   ::StrStrI(szModulePath, TEXT("boncasproxy.exe"	)) ||
	   ::StrStrI(szModulePath, TEXT("boncasservice.exe"	)) ||
	   ::StrStrI(szModulePath, TEXT("system32"			))){
		return FALSE;
		}
	
	return TRUE;
}

static const BOOL InstallKernelHook(void)
{
	HMODULE hCurModule = ::GetModuleHandle(TEXT("Kernel32.dll"));
	if(!hCurModule)return FALSE;

	// オリジナルのエントリーポイントを退避
	pfnLoadLibraryWOrg = (PFNLOADLIBRARYW)::GetProcAddress(hCurModule, "LoadLibraryW");
	pfnLoadLibraryAOrg = (PFNLOADLIBRARYA)::GetProcAddress(hCurModule, "LoadLibraryA");

	if(!pfnLoadLibraryWOrg || !pfnLoadLibraryAOrg)return FALSE;

	// IATを上書き
	::ReplaceApiProc("Kernel32.dll", (PROC)pfnLoadLibraryWOrg, (PROC)::LoadLibraryWHook);
	::ReplaceApiProc("Kernel32.dll", (PROC)pfnLoadLibraryAOrg, (PROC)::LoadLibraryAHook);
	
	return TRUE;
}

static const BOOL RemoveKernelHook(void)
{
	if(!pfnLoadLibraryWOrg || !pfnLoadLibraryAOrg)return FALSE;

	// IATをオリジナルに戻す
	::ReplaceApiProc("Kernel32.dll", (PROC)::LoadLibraryWHook, (PROC)pfnLoadLibraryWOrg);
	::ReplaceApiProc("Kernel32.dll", (PROC)::LoadLibraryAHook, (PROC)pfnLoadLibraryAOrg);

	return TRUE;
}

static const BOOL InstallScardHook(void)
{
	const HMODULE hCurModule = ::GetModuleHandle(TEXT("WinScard.dll"));
	if(!hCurModule)return FALSE;

	// IATを上書き
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardConnectA"		  ), (PROC)::SCardConnectAHook			);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardConnectW"		  ), (PROC)::SCardConnectWHook			);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardDisconnect"		  ), (PROC)::SCardDisconnectHook		);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardEstablishContext" ), (PROC)::SCardEstablishContextHook	);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardFreeMemory"		  ), (PROC)::SCardFreeMemoryHook		);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardListReadersA"	  ), (PROC)::SCardListReadersAHook		);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardListReadersW"	  ), (PROC)::SCardListReadersWHook		);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardTransmit"		  ), (PROC)::SCardTransmitHook			);
	::ReplaceApiProc("WinScard.dll", ::GetProcAddress(hCurModule, "SCardReleaseContext"	  ), (PROC)::SCardReleaseContextHook	);

	return TRUE;
}

static HANDLE WINAPI LoadLibraryWHook(LPCWSTR lpLibFileName)
{
	const HANDLE hReturn = pfnLoadLibraryWOrg(lpLibFileName);

	// WinScard.dll APIフック
	if(::InstallScardHook()){
		// イベント送信
		CCasProxy::SendEnterProcessEvent(hHostHwnd);
		}

	return hReturn;
}

static HANDLE WINAPI LoadLibraryAHook(LPCSTR lpLibFileName)
{
	const HANDLE hReturn = pfnLoadLibraryAOrg(lpLibFileName);

	// WinScard.dll APIフック
	if(::InstallScardHook()){
		// イベント送信
		CCasProxy::SendEnterProcessEvent(hHostHwnd);
		}

	return hReturn;
}


//////////////////////////////////////////////////////////////////////
// システムフック
//////////////////////////////////////////////////////////////////////

LRESULT CALLBACK CbtHookProc(int iCode, WPARAM wParam, LPARAM lParam)
{
	// フックプロシージャ(何もしない)
	return ::CallNextHookEx(hHook, iCode, wParam, lParam);
}

extern "C" __declspec(dllexport) BOOL InstallHook(HWND hWnd)
{
	// システムフックをインストール
	hHostHwnd = hWnd;
	hHook = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CbtHookProc, hInstance, 0UL);

	return (hHook)? TRUE : FALSE;
}

extern "C" __declspec(dllexport) BOOL RemoveHook(void)
{
	// システムフックをアンインストール
	return (::UnhookWindowsHookEx(hHook))? TRUE : FALSE;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif
