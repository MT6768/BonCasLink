// BonCasServerDlg.h : ヘッダー ファイル
//

#pragma once


#include "CasServer.h"
#include "NotifyIcon.h"
#include "ConfigData.h"


// CBonCasServerDlg ダイアログ
class CBonCasServerDlg : public CDialog,
						 protected CCasServer::ICasServerHandler,
						 protected CNotifyIcon::INotifyIconHandler						 
{
// コンストラクション
public:
	CBonCasServerDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_BONCASSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
	void OnOK(void);
	void OnCancel(void);

	void UpdateToolTip(void);
	void ShowContextMenu(void);

	virtual void OnCasServerEvent(CCasServer *pCasServer, const WORD wEventID);
	virtual void OnNotifyIconEvent(CNotifyIcon *pNotifyIcon, const WORD wEventID);

	HICON m_hIcon;
	CCasServer m_CasServer;
	CConfigDword m_dwServerPort;
	CNotifyIcon m_NotifyIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnMenuSetup();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuExit();
	DECLARE_MESSAGE_MAP()
};
