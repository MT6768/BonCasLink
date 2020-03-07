// NotifyIcon.cpp: CNotifyIcon �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "NotifyIcon.h"


#define WM_NOTIFYICON	(WM_USER + 101UL)


CNotifyIcon::CNotifyIcon(INotifyIconHandler *pEventHandler)
	: m_pEventHandler(pEventHandler)
	, m_hHostHwnd(NULL)
	, m_OldHostWndProc(NULL)
{

}

CNotifyIcon::~CNotifyIcon(void)
{
	RemoveIcon();
}

const BOOL CNotifyIcon::AddIcon(const HWND hHwnd, const HICON hIcon, LPCTSTR lpszTip)
{
	m_hHostHwnd = hHwnd;

	// �^�X�N�g���C�ɃA�C�R���ǉ�
	NOTIFYICONDATA IconData;
	IconData.cbSize = sizeof(IconData);
	IconData.uID = 1UL;
	IconData.hWnd = hHwnd;
	IconData.uFlags = NIF_MESSAGE | NIF_ICON | ((lpszTip)? NIF_TIP : 0UL);
	IconData.hIcon = hIcon;
	IconData.uCallbackMessage = WM_NOTIFYICON;
	IconData.uVersion = NOTIFYICON_VERSION;

	if(lpszTip)::lstrcpy(IconData.szTip, lpszTip);
	if(!::Shell_NotifyIcon(NIM_ADD, &IconData))return FALSE;
	if(!::Shell_NotifyIcon(NIM_SETVERSION, &IconData))return FALSE;
	
	// �z�X�g�E�B���h�E���T�u�N���X������
	if(!(m_OldHostWndProc = (WNDPROC)::SetWindowLongPtr(hHwnd, GWLP_WNDPROC, (LONG_PTR)CNotifyIcon::HostWndProcRaw)))return FALSE;
	::SetWindowLongPtr(hHwnd, GWLP_USERDATA, (LONG_PTR)this);

	return TRUE;
}

const BOOL CNotifyIcon::RemoveIcon(void)
{
	if(!m_hHostHwnd)return FALSE;

	// �T�u�N���X������	
	::SetWindowLongPtr(m_hHostHwnd, GWLP_WNDPROC, (LONG_PTR)m_OldHostWndProc);

	// �^�X�N�g���C����A�C�R�����폜
	NOTIFYICONDATA IconData;
	IconData.cbSize = sizeof(IconData);
	IconData.uID = 1UL;
	IconData.hWnd = m_hHostHwnd;
	IconData.uFlags = 0UL;

	m_hHostHwnd = NULL;

	return ::Shell_NotifyIcon(NIM_DELETE, &IconData);
}

const BOOL CNotifyIcon::SetIcon(const HICON hIcon)
{
	if(!m_hHostHwnd || !hIcon)return FALSE;

	// �^�X�N�g���C�̃A�C�R���ύX
	NOTIFYICONDATA IconData;
	IconData.cbSize = sizeof(IconData);
	IconData.uID = 1UL;
	IconData.uFlags = NIF_ICON;
	IconData.hWnd = m_hHostHwnd;
	IconData.hIcon = hIcon;

	return ::Shell_NotifyIcon(NIM_MODIFY, &IconData);
}

const BOOL CNotifyIcon::SetTip(LPCTSTR lpszTip)
{
	if(!m_hHostHwnd || !lpszTip)return FALSE;

	// �^�X�N�g���C�̃c�[���`�b�v�e�L�X�g�ύX
	NOTIFYICONDATA IconData;
	IconData.cbSize = sizeof(IconData);
	IconData.uID = 1UL;
	IconData.hWnd = m_hHostHwnd;
	IconData.uFlags = NIF_TIP;

	::lstrcpy(IconData.szTip, lpszTip);

	return ::Shell_NotifyIcon(NIM_MODIFY, &IconData);
}

const BOOL CNotifyIcon::ShowBalloon(LPCTSTR lpszTitle, LPCTSTR lpszMessage, const DWORD dwBalloonType)
{
	if(!m_hHostHwnd || !lpszTitle || !lpszMessage)return FALSE;

	// �o���[����\������
	NOTIFYICONDATA IconData;
	IconData.cbSize = sizeof(IconData);
	IconData.uID = 1UL;
	IconData.hWnd = m_hHostHwnd;
	IconData.uFlags = NIF_INFO;

	IconData.uTimeout = 6000UL;

	switch(dwBalloonType){
		case BAL_ERROR :	IconData.dwInfoFlags = NIIF_ERROR;		break;
		case BAL_INFO :		IconData.dwInfoFlags = NIIF_INFO;		break;
		case BAL_WARNING :	IconData.dwInfoFlags = NIIF_WARNING;	break;
		default :			IconData.dwInfoFlags = NIIF_INFO;		break;
		}
	
	::lstrcpy(IconData.szInfoTitle, lpszTitle);
	::lstrcpy(IconData.szInfo, lpszMessage);

	return ::Shell_NotifyIcon(NIM_MODIFY, &IconData);
}

const BOOL CNotifyIcon::HideBalloon(void)
{
	if(!m_hHostHwnd)return FALSE;

	// �o���[�������
	NOTIFYICONDATA IconData;
	IconData.cbSize = sizeof(IconData);
	IconData.uID = 1UL;
	IconData.hWnd = m_hHostHwnd;
	IconData.uFlags = NIF_INFO;
	IconData.dwInfoFlags = NIIF_INFO;
	IconData.uTimeout = 6000UL;
	
	IconData.szInfoTitle[0] = TEXT('\0');
	IconData.szInfo[0] = TEXT('\0');

	return ::Shell_NotifyIcon(NIM_MODIFY, &IconData);
}

LRESULT CALLBACK CNotifyIcon::HostWndProcRaw(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CNotifyIcon *pThis = reinterpret_cast<CNotifyIcon *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

	// �����₷���悤�ɃC���X�^���X���̃����o���Ă�
	if(pThis->HostWndProc(uMsg, wParam, lParam))return TRUE;

	return ::CallWindowProc(pThis->m_OldHostWndProc, hwnd, uMsg, wParam, lParam);
}

const BOOL CNotifyIcon::HostWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_NOTIFYICON){
		// �C�x���g�n���h�����Ăяo��
		m_pEventHandler->OnNotifyIconEvent(this, LOWORD(lParam));
		return TRUE;
		}
		
	return FALSE;	
}
