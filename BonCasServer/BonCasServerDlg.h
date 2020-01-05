// BonCasServerDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once


#include "CasServer.h"
#include "NotifyIcon.h"
#include "ConfigData.h"


// CBonCasServerDlg �_�C�A���O
class CBonCasServerDlg : public CDialog,
						 protected CCasServer::ICasServerHandler,
						 protected CNotifyIcon::INotifyIconHandler						 
{
// �R���X�g���N�V����
public:
	CBonCasServerDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^

// �_�C�A���O �f�[�^
	enum { IDD = IDD_BONCASSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g

// ����
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

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
