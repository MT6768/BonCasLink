// BonCasProxy.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "BonCasProxyDlg.h"


// CBonCasProxyApp:
// このクラスの実装については、BonCasProxy.cpp を参照してください。
//

class CBonCasProxyApp : public CWinApp
{
public:
	CBonCasProxyApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装
	CBonCasProxyDlg *m_pBonCasProxyDlg;

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBonCasProxyApp theApp;