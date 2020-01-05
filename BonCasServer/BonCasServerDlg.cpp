// BonCasServerDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "BonCasServer.h"
#include "BonCasServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CBonCasServerDlg ダイアログ




CBonCasServerDlg::CBonCasServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBonCasServerDlg::IDD, pParent)
	, m_CasServer(this)
	, m_NotifyIcon(this)
	, m_dwServerPort(CONFSECT_GENERAL, TEXT("ServerPort"), 6900UL, 65535UL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBonCasServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SERVERPORT, m_dwServerPort);
	DDV_MinMaxUInt(pDX, m_dwServerPort, 0, 65535);
}

BEGIN_MESSAGE_MAP(CBonCasServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_SETUP, &CBonCasServerDlg::OnMenuSetup)
	ON_COMMAND(ID_MENU_ABOUT, &CBonCasServerDlg::OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, &CBonCasServerDlg::OnMenuExit)
END_MESSAGE_MAP()


// CBonCasServerDlg メッセージ ハンドラ

BOOL CBonCasServerDlg::OnInitDialog()
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

	// サーバオープン
	if(!m_CasServer.OpenServer((WORD)m_dwServerPort)){
		::AfxMessageBox(TEXT("サーバの起動に失敗しました。\r\nTCPポートまたはカードリーダをオープンできません。"));
		return TRUE;
		}

	// タスクトレイにアイコン追加
	m_NotifyIcon.AddIcon(GetSafeHwnd(), ::AfxGetApp()->LoadIcon(IDI_BLUECARD), APP_TITLE TEXT("\r\n接続中のクライアント： 0"));

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CBonCasServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBonCasServerDlg::OnPaint()
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
HCURSOR CBonCasServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBonCasServerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
	m_CasServer.CloseServer();

	// タスクトレイのアイコン削除
	m_NotifyIcon.RemoveIcon();
}

void CBonCasServerDlg::OnOK(void)
{
	const DWORD dwPrevPort = m_dwServerPort;

	if(!UpdateData(TRUE))return;

	if((m_dwServerPort != dwPrevPort) && m_CasServer.GetClientNum()){
		if(::AfxMessageBox(TEXT("接続中のクライアントが存在します。これらを切断して設定を反映しますか？\r\n\r\n「いいえ」を選択した場合は次回起動時に反映されます。"), MB_YESNO) == IDYES){
			
			// サーバ再起動
			m_CasServer.CloseServer();
			if(!m_CasServer.OpenServer((WORD)m_dwServerPort)){
				::AfxMessageBox(TEXT("サーバの起動に失敗しました。\r\nTCPポートまたはカードリーダをオープンできません。"));
				return;
				}
	
			}		
		}

	ShowWindow(SW_HIDE);
}

void CBonCasServerDlg::OnCancel(void)
{
	UpdateData(FALSE);
	ShowWindow(SW_HIDE);
}

void CBonCasServerDlg::UpdateToolTip(void)
{
	CString csText;
	csText.Format(TEXT("%s\r\n接続中のクライアント： %ld"), APP_TITLE, m_CasServer.GetClientNum());

	m_NotifyIcon.SetTip(csText);
}

void CBonCasServerDlg::ShowContextMenu(void)
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

void CBonCasServerDlg::OnCasServerEvent(CCasServer *pCasServer, const WORD wEventID)
{
	switch(wEventID){
		case CCasServer::CSEI_CONNECTED :
		case CCasServer::CSEI_DISCONNECTED :
			m_NotifyIcon.SetIcon(::AfxGetApp()->LoadIcon((pCasServer->GetClientNum())? IDI_REDCARD : IDI_BLUECARD));
			UpdateToolTip();
			break;
		}
}

void CBonCasServerDlg::OnNotifyIconEvent(CNotifyIcon *pNotifyIcon, const WORD wEventID)
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
		}
}

void CBonCasServerDlg::OnMenuSetup()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	ShowWindow(SW_SHOW);
	SetForegroundWindow();
}

void CBonCasServerDlg::OnMenuAbout()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CAboutDlg AboutDlg;
	AboutDlg.DoModal();
}

void CBonCasServerDlg::OnMenuExit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	if(m_CasServer.GetClientNum()){
		if(::AfxMessageBox(TEXT("接続中のクライアントが存在します。\r\n終了しますか？"), MB_YESNO | MB_ICONEXCLAMATION) != IDYES)return;
		}

	DestroyWindow();
}
