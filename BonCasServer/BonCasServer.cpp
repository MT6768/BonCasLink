// BonCasServer.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "BonCasServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBonCasServerApp

BEGIN_MESSAGE_MAP(CBonCasServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBonCasServerApp コンストラクション

CBonCasServerApp::CBonCasServerApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CBonCasServerApp オブジェクトです。

CBonCasServerApp theApp;


// CBonCasServerApp 初期化

BOOL CBonCasServerApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	m_pBonCasServerDlg = new CBonCasServerDlg;
	m_pBonCasServerDlg->Create(CBonCasServerDlg::IDD);
	m_pMainWnd = m_pBonCasServerDlg;

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return TRUE;
}

int CBonCasServerApp::ExitInstance()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	delete m_pBonCasServerDlg;

	return CWinApp::ExitInstance();
}
