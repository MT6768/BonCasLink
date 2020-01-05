// BonCasProxyDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxcmn.h"
#include "NotifyIcon.h"
#include "ConfigData.h"


// CBonCasProxyDlg ダイアログ
class CBonCasProxyDlg : public CDialog,
						protected CNotifyIcon::INotifyIconHandler
{
// コンストラクション
public:
	CBonCasProxyDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_BONCASPROXY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	void OnOK(void);
	void OnCancel(void);

	void UpdateToolTip(void);
	void AddClient(const DWORD dwPID);
	void RemoveClient(const DWORD dwPID);

	static CString GetProcessPath(const DWORD dwPID);
	static CString GetProcessName(const DWORD dwPID);

	virtual void OnNotifyIconEvent(CNotifyIcon *pNotifyIcon, const WORD wEventID);
	void ShowContextMenu(void);

	HICON m_hIcon;
	CConfigDword m_dwServerAddr;
	CConfigDword m_dwServerPort;
	CListCtrl m_ClientList;
	CNotifyIcon m_NotifyIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnClientEvent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnMenuSetup();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuExit();
	DECLARE_MESSAGE_MAP()
};
