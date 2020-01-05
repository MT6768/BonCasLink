// BonCasServer.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "BonCasServerDlg.h"


// CBonCasServerApp:
// このクラスの実装については、BonCasServer.cpp を参照してください。
//

class CBonCasServerApp : public CWinApp
{
public:
	CBonCasServerApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装
	CBonCasServerDlg *m_pBonCasServerDlg;

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBonCasServerApp theApp;