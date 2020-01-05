// NotifyIcon.h: CNotifyIcon クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////


#pragma once


class CNotifyIcon
{
public:
	class INotifyIconHandler
	{
	public:
		virtual void OnNotifyIconEvent(CNotifyIcon *pNotifyIcon, const WORD wEventID) = 0;
	};

	enum
	{
		BAL_ERROR, BAL_INFO, BAL_WARNING
	};

	CNotifyIcon(INotifyIconHandler *pEventHandler);
	~CNotifyIcon(void);

	const BOOL AddIcon(const HWND hHwnd, const HICON hIcon, LPCTSTR lpszTip = NULL);
	const BOOL RemoveIcon(void);
	const BOOL SetIcon(const HICON hIcon);
	const BOOL SetTip(LPCTSTR lpszTip);
	const BOOL ShowBalloon(LPCTSTR lpszTitle, LPCTSTR lpszMessage, const DWORD dwBalloonType = BAL_INFO);
	const BOOL HideBalloon(void);

protected:
	static LRESULT CALLBACK HostWndProcRaw(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	const BOOL HostWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	INotifyIconHandler *m_pEventHandler;
	HWND m_hHostHwnd;
	WNDPROC m_OldHostWndProc;
};
