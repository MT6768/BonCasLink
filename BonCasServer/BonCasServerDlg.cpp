// BonCasServerDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "BonCasServer.h"
#include "BonCasServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
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


// CBonCasServerDlg �_�C�A���O




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


// CBonCasServerDlg ���b�Z�[�W �n���h��

BOOL CBonCasServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
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

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	// �T�[�o�I�[�v��
	if(!m_CasServer.OpenServer((WORD)m_dwServerPort)){
		::AfxMessageBox(TEXT("�T�[�o�̋N���Ɏ��s���܂����B\r\nTCP�|�[�g�܂��̓J�[�h���[�_���I�[�v���ł��܂���B"));
		return TRUE;
		}

	// �^�X�N�g���C�ɃA�C�R���ǉ�
	m_NotifyIcon.AddIcon(GetSafeHwnd(), ::AfxGetApp()->LoadIcon(IDI_BLUECARD), APP_TITLE TEXT("\r\n�ڑ����̃N���C�A���g�F 0"));

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CBonCasServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CBonCasServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBonCasServerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
	m_CasServer.CloseServer();

	// �^�X�N�g���C�̃A�C�R���폜
	m_NotifyIcon.RemoveIcon();
}

void CBonCasServerDlg::OnOK(void)
{
	const DWORD dwPrevPort = m_dwServerPort;

	if(!UpdateData(TRUE))return;

	if((m_dwServerPort != dwPrevPort) && m_CasServer.GetClientNum()){
		if(::AfxMessageBox(TEXT("�ڑ����̃N���C�A���g�����݂��܂��B������ؒf���Đݒ�𔽉f���܂����H\r\n\r\n�u�������v��I�������ꍇ�͎���N�����ɔ��f����܂��B"), MB_YESNO) == IDYES){
			
			// �T�[�o�ċN��
			m_CasServer.CloseServer();
			if(!m_CasServer.OpenServer((WORD)m_dwServerPort)){
				::AfxMessageBox(TEXT("�T�[�o�̋N���Ɏ��s���܂����B\r\nTCP�|�[�g�܂��̓J�[�h���[�_���I�[�v���ł��܂���B"));
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
	csText.Format(TEXT("%s\r\n�ڑ����̃N���C�A���g�F %ld"), APP_TITLE, m_CasServer.GetClientNum());

	m_NotifyIcon.SetTip(csText);
}

void CBonCasServerDlg::ShowContextMenu(void)
{
	// �|�b�v�A�b�v���j���[��\��
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
	// �^�X�N�g���C�A�C�R���C�x���g
	switch(wEventID){
		case WM_LBUTTONUP :
			// ���N���b�N
			ShowContextMenu();
			break;
		
		case WM_RBUTTONUP :
			// �E�N���b�N
			ShowContextMenu();
			break;
		}
}

void CBonCasServerDlg::OnMenuSetup()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	ShowWindow(SW_SHOW);
	SetForegroundWindow();
}

void CBonCasServerDlg::OnMenuAbout()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	CAboutDlg AboutDlg;
	AboutDlg.DoModal();
}

void CBonCasServerDlg::OnMenuExit()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	if(m_CasServer.GetClientNum()){
		if(::AfxMessageBox(TEXT("�ڑ����̃N���C�A���g�����݂��܂��B\r\n�I�����܂����H"), MB_YESNO | MB_ICONEXCLAMATION) != IDYES)return;
		}

	DestroyWindow();
}
