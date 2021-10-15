// BonCasProxyDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include <stdlib.h>
#include <psapi.h>
#include "BonCasProxy.h"
#include "BonCasProxyDlg.h"
#include "..\BonCasStub\CasProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment(lib, "BonCasStub.lib")
#pragma comment(lib, "psapi.lib")


extern "C" __declspec(dllimport) BOOL InstallHook(HWND hWnd);
extern "C" __declspec(dllimport) BOOL RemoveHook(void);



// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CBonCasProxyDlg ダイアログ




CBonCasProxyDlg::CBonCasProxyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBonCasProxyDlg::IDD, pParent)
	, m_NotifyIcon(this)
	, m_dwServerAddr(CONFSECT_GENERAL, TEXT("ServerAddr"), 0x7F000001UL)
	, m_dwServerPort(CONFSECT_GENERAL, TEXT("ServerPort"), 6900UL, 65535UL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBonCasProxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_SERVERADDR, m_dwServerAddr);
	DDX_Text(pDX, IDC_SERVERPORT, m_dwServerPort);
	DDV_MinMaxUInt(pDX, m_dwServerPort, 0, 65535);
	DDX_Control(pDX, IDC_CLIENTLIST, m_ClientList);
}

BEGIN_MESSAGE_MAP(CBonCasProxyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER + 100U, OnClientEvent)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_SETUP, &CBonCasProxyDlg::OnMenuSetup)
	ON_COMMAND(ID_MENU_ABOUT, &CBonCasProxyDlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, &CBonCasProxyDlg::OnMenuExit)
END_MESSAGE_MAP()


// CBonCasProxyDlg メッセージ ハンドラ

BOOL CBonCasProxyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	m_ClientList.InsertColumn(0, TEXT("プロセスID"), LVCFMT_LEFT, 60);
	m_ClientList.InsertColumn(1, TEXT("ファイルパス"), LVCFMT_LEFT, 400);

	// タスクトレイにアイコン追加
	m_NotifyIcon.AddIcon(GetSafeHwnd(), ::AfxGetApp()->LoadIcon(IDI_BLUECARD), APP_TITLE TEXT("\r\n接続中のクライアント： 0"));

	// フックインストール
	::InstallHook(GetSafeHwnd());

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CBonCasProxyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CBonCasProxyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CBonCasProxyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBonCasProxyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。

	// フックアンインストール
	::RemoveHook();

	// タスクトレイのアイコン削除
	m_NotifyIcon.RemoveIcon();
}

void CBonCasProxyDlg::OnOK(void)
{
	if(!UpdateData(TRUE))return;

	if(((m_dwServerAddr >> 24) != 192UL) && ((m_dwServerAddr >> 24) != 127UL)){
		::AfxMessageBox(TEXT("ローカルでないIPアドレスが指定されています。\r\n\r\nスマートカードには個人を特定可能な情報が含まれている可能性があります。\r\n本ソフトウェアは通信の暗号化を行わないためそのリスクを十分に理解した上で\r\nご使用ください。"));
		}
		
	ShowWindow(SW_HIDE);
}

void CBonCasProxyDlg::OnCancel(void)
{
	UpdateData(FALSE);
	ShowWindow(SW_HIDE);
}

void CBonCasProxyDlg::UpdateToolTip(void)
{
	CString csText;
	csText.Format(TEXT("%s\r\n接続中のアプリケーション： %ld"), APP_TITLE, m_ClientList.GetItemCount());

	m_NotifyIcon.SetTip(csText);
}

void CBonCasProxyDlg::AddClient(const DWORD dwPID)
{
	// 既存のPIDを検索する
	for(int iIndex = 0 ; iIndex < m_ClientList.GetItemCount() ; iIndex++){
		if(m_ClientList.GetItemData(iIndex) == dwPID)return;
		}

	// アイテムを追加
	CString csPID;
	csPID.Format(TEXT("%08X"), dwPID);
	const int iPos = m_ClientList.InsertItem(m_ClientList.GetItemCount(), csPID);
	m_ClientList.SetItemText(iPos, 1, GetProcessPath(dwPID));
	m_ClientList.SetItemData(iPos, dwPID);
	
	// タスクトレイアイコンを変更
	m_NotifyIcon.SetIcon(::AfxGetApp()->LoadIcon(IDI_REDCARD));
	
	// ツールチップのテキスト更新
	UpdateToolTip();
}

void CBonCasProxyDlg::RemoveClient(const DWORD dwPID)
{
	// アイテムを削除
	for(int iIndex = 0 ; iIndex < m_ClientList.GetItemCount() ; iIndex++){
		if(m_ClientList.GetItemData(iIndex) == dwPID){
			m_ClientList.DeleteItem(iIndex);
			break;
			}
		}

	// タスクトレイアイコンを変更
	if(!m_ClientList.GetItemCount())m_NotifyIcon.SetIcon(::AfxGetApp()->LoadIcon(IDI_BLUECARD));

	// ツールチップのテキスト更新
	UpdateToolTip();
}

CString CBonCasProxyDlg::GetProcessPath(const DWORD dwPID)
{
	// PIDからパスを取得
	TCHAR szModulePath[_MAX_PATH] = TEXT("不明なアプリケーション");
	HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);

	if(hProcess){
		HMODULE hModule = NULL;
		DWORD dwReqSize = 1UL;
		if(::EnumProcessModules(hProcess, &hModule, 1UL, &dwReqSize)){
			::GetModuleFileNameEx(hProcess, hModule, szModulePath, sizeof(szModulePath));
			}

		::CloseHandle(hProcess);
		}

	return CString(szModulePath);
}

CString CBonCasProxyDlg::GetProcessName(const DWORD dwPID)
{
	TCHAR szModuleName[_MAX_FNAME] = {TEXT('\0')};
	TCHAR szModuleExt[_MAX_EXT] = {TEXT('\0')};

	// フルパスを分割
	::_tsplitpath(GetProcessPath(dwPID), NULL, NULL, szModuleName, szModuleExt);
	lstrcat(szModuleName, TEXT("."));
	lstrcat(szModuleName, szModuleExt);

	return CString(szModuleName);
}

LRESULT CBonCasProxyDlg::OnClientEvent(WPARAM wParam, LPARAM lParam)
{
	// BonCasStubからのイベント
	switch(wParam){
		case CCasProxy::CPEI_ENTERPROCESS :		// プロセス開始
			AddClient((DWORD)lParam);
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_EXITPROCESS :		// プロセス終了
			RemoveClient((DWORD)lParam);
			m_NotifyIcon.HideBalloon();
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_GETSERVERIP :		// サーバIP取得
			UpdateData(TRUE);
			return m_dwServerAddr;

		case CCasProxy::CPEI_GETSERVERPORT :	// サーバポート取得
			UpdateData(TRUE);
			return (LRESULT)m_dwServerPort;

		case CCasProxy::CPEI_CONNECTSUCCESS :	// 接続完了
			m_NotifyIcon.HideBalloon();
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_CONNECTFAILED :	// 接続タイムアウト
			m_NotifyIcon.ShowBalloon(APP_TITLE, TEXT("BonCasServerに接続できません\r\nサーバの設定を確認してください。\r\n\r\nクリックすると設定画面を表示します。"), CNotifyIcon::BAL_ERROR);
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_DISCONNECTED:		// 接続切断
			m_NotifyIcon.ShowBalloon(APP_TITLE, TEXT("BonCasServerから通信を切断されました。"), CNotifyIcon::BAL_WARNING);
			return (LRESULT)TRUE;
		}

	return (LRESULT)FALSE;
}

void CBonCasProxyDlg::OnNotifyIconEvent(CNotifyIcon *pNotifyIcon, const WORD wEventID)
{
	// タスクトレイアイコンイベント
	switch(wEventID){
		case WM_LBUTTONUP :
			// 左クリック
			ShowContextMenu();
			break;
		
		case WM_RBUTTONUP :
			// 右クリック
			ShowContextMenu();
			break;

		case NIN_BALLOONUSERCLICK :
			// バルーンクリック
			OnMenuSetup();
			break;
		}
}

void CBonCasProxyDlg::ShowContextMenu(void)
{
	// ポップアップメニューを表示
	CPoint ptPos;
	::GetCursorPos(&ptPos);

	CMenu BaseMenu;
	BaseMenu.LoadMenu(IDR_MENU);
	CMenu *pPopupMenu = BaseMenu.GetSubMenu(0);
	
	SetForegroundWindow();
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, ptPos.x, ptPos.y, this);
	BaseMenu.DestroyMenu();
	PostMessage(WM_USER);
}

void CBonCasProxyDlg::OnMenuSetup()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	ShowWindow(SW_SHOW);
	SetForegroundWindow();
}

void CBonCasProxyDlg::OnMenuAbout()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CAboutDlg AboutDlg;
	AboutDlg.DoModal();
}

void CBonCasProxyDlg::OnMenuExit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	if(m_ClientList.GetItemCount()){
		if(::AfxMessageBox(TEXT("カードリーダを使用中のアプリケーションが存在します。\r\nBonCasProxyを終了するとこれらが異常終了します。\r\n\r\n強制的に終了しますか？"), MB_YESNO | MB_ICONEXCLAMATION) != IDYES)return;
		}

	DestroyWindow();
}
