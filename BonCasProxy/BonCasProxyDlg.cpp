// BonCasProxyDlg.cpp : �����t�@�C��
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


#ifdef _DEBUG
	#pragma comment(lib, "..\\Debug\\BonCasStub.lib")
#else
	#pragma comment(lib, "..\\Release\\BonCasStub.lib")
#endif

#pragma comment(lib, "psapi.lib")


extern "C" __declspec(dllimport) BOOL InstallHook(HWND hWnd);
extern "C" __declspec(dllimport) BOOL RemoveHook(void);



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


// CBonCasProxyDlg �_�C�A���O




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


// CBonCasProxyDlg ���b�Z�[�W �n���h��

BOOL CBonCasProxyDlg::OnInitDialog()
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
	m_ClientList.InsertColumn(0, TEXT("�v���Z�XID"), LVCFMT_LEFT, 60);
	m_ClientList.InsertColumn(1, TEXT("�t�@�C���p�X"), LVCFMT_LEFT, 400);

	// �^�X�N�g���C�ɃA�C�R���ǉ�
	m_NotifyIcon.AddIcon(GetSafeHwnd(), ::AfxGetApp()->LoadIcon(IDI_BLUECARD), APP_TITLE TEXT("\r\n�ڑ����̃N���C�A���g�F 0"));

	// �t�b�N�C���X�g�[��
	::InstallHook(GetSafeHwnd());

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CBonCasProxyDlg::OnPaint()
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
HCURSOR CBonCasProxyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBonCasProxyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B

	// �t�b�N�A���C���X�g�[��
	::RemoveHook();

	// �^�X�N�g���C�̃A�C�R���폜
	m_NotifyIcon.RemoveIcon();
}

void CBonCasProxyDlg::OnOK(void)
{
	if(!UpdateData(TRUE))return;

	if(((m_dwServerAddr >> 24) != 192UL) && ((m_dwServerAddr >> 24) != 127UL)){
		::AfxMessageBox(TEXT("���[�J���łȂ�IP�A�h���X���w�肳��Ă��܂��B\r\n\r\n�X�}�[�g�J�[�h�ɂ͌l�����\�ȏ�񂪊܂܂�Ă���\��������܂��B\r\n�{�\�t�g�E�F�A�͒ʐM�̈Í������s��Ȃ����߂��̃��X�N���\���ɗ����������\r\n���g�p���������B"));
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
	csText.Format(TEXT("%s\r\n�ڑ����̃A�v���P�[�V�����F %ld"), APP_TITLE, m_ClientList.GetItemCount());

	m_NotifyIcon.SetTip(csText);
}

void CBonCasProxyDlg::AddClient(const DWORD dwPID)
{
	// ������PID����������
	for(int iIndex = 0 ; iIndex < m_ClientList.GetItemCount() ; iIndex++){
		if(m_ClientList.GetItemData(iIndex) == dwPID)return;
		}

	// �A�C�e����ǉ�
	CString csPID;
	csPID.Format(TEXT("%08X"), dwPID);
	const int iPos = m_ClientList.InsertItem(m_ClientList.GetItemCount(), csPID);
	m_ClientList.SetItemText(iPos, 1, GetProcessPath(dwPID));
	m_ClientList.SetItemData(iPos, dwPID);
	
	// �^�X�N�g���C�A�C�R����ύX
	m_NotifyIcon.SetIcon(::AfxGetApp()->LoadIcon(IDI_REDCARD));
	
	// �c�[���`�b�v�̃e�L�X�g�X�V
	UpdateToolTip();
}

void CBonCasProxyDlg::RemoveClient(const DWORD dwPID)
{
	// �A�C�e�����폜
	for(int iIndex = 0 ; iIndex < m_ClientList.GetItemCount() ; iIndex++){
		if(m_ClientList.GetItemData(iIndex) == dwPID){
			m_ClientList.DeleteItem(iIndex);
			break;
			}
		}

	// �^�X�N�g���C�A�C�R����ύX
	if(!m_ClientList.GetItemCount())m_NotifyIcon.SetIcon(::AfxGetApp()->LoadIcon(IDI_BLUECARD));

	// �c�[���`�b�v�̃e�L�X�g�X�V
	UpdateToolTip();
}

CString CBonCasProxyDlg::GetProcessPath(const DWORD dwPID)
{
	// PID����p�X���擾
	TCHAR szModulePath[_MAX_PATH] = TEXT("�s���ȃA�v���P�[�V����");
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

	// �t���p�X�𕪊�
	::_tsplitpath(GetProcessPath(dwPID), NULL, NULL, szModuleName, szModuleExt);
	lstrcat(szModuleName, TEXT("."));
	lstrcat(szModuleName, szModuleExt);

	return CString(szModuleName);
}

LRESULT CBonCasProxyDlg::OnClientEvent(WPARAM wParam, LPARAM lParam)
{
	// BonCasStub����̃C�x���g
	switch(wParam){
		case CCasProxy::CPEI_ENTERPROCESS :		// �v���Z�X�J�n
			AddClient((DWORD)lParam);
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_EXITPROCESS :		// �v���Z�X�I��
			RemoveClient((DWORD)lParam);
			m_NotifyIcon.HideBalloon();
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_GETSERVERIP :		// �T�[�oIP�擾
			UpdateData(TRUE);
			return m_dwServerAddr;

		case CCasProxy::CPEI_GETSERVERPORT :	// �T�[�o�|�[�g�擾
			UpdateData(TRUE);
			return (LRESULT)m_dwServerPort;

		case CCasProxy::CPEI_CONNECTSUCCESS :	// �ڑ�����
			m_NotifyIcon.HideBalloon();
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_CONNECTFAILED :	// �ڑ��^�C���A�E�g
			m_NotifyIcon.ShowBalloon(APP_TITLE, TEXT("BonCasServer�ɐڑ��ł��܂���\r\n�T�[�o�̐ݒ���m�F���Ă��������B\r\n\r\n�N���b�N����Ɛݒ��ʂ�\�����܂��B"), CNotifyIcon::BAL_ERROR);
			return (LRESULT)TRUE;

		case CCasProxy::CPEI_DISCONNECTED:		// �ڑ��ؒf
			m_NotifyIcon.ShowBalloon(APP_TITLE, TEXT("BonCasServer����ʐM��ؒf����܂����B"), CNotifyIcon::BAL_WARNING);
			return (LRESULT)TRUE;
		}

	return (LRESULT)FALSE;
}

void CBonCasProxyDlg::OnNotifyIconEvent(CNotifyIcon *pNotifyIcon, const WORD wEventID)
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

		case NIN_BALLOONUSERCLICK :
			// �o���[���N���b�N
			OnMenuSetup();
			break;
		}
}

void CBonCasProxyDlg::ShowContextMenu(void)
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

void CBonCasProxyDlg::OnMenuSetup()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	ShowWindow(SW_SHOW);
	SetForegroundWindow();
}

void CBonCasProxyDlg::OnMenuAbout()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	CAboutDlg AboutDlg;
	AboutDlg.DoModal();
}

void CBonCasProxyDlg::OnMenuExit()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	if(m_ClientList.GetItemCount()){
		if(::AfxMessageBox(TEXT("�J�[�h���[�_���g�p���̃A�v���P�[�V���������݂��܂��B\r\nBonCasProxy���I������Ƃ���炪�ُ�I�����܂��B\r\n\r\n�����I�ɏI�����܂����H"), MB_YESNO | MB_ICONEXCLAMATION) != IDYES)return;
		}

	DestroyWindow();
}
