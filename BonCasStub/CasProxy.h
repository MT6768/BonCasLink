// CasProxy.h: CCasProxy �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////


#pragma once


#include "SmartSock.h"


class CCasProxy
{
public:
	enum
	{
		CPEI_ENTERPROCESS,		// �v���Z�X�J�n�ʒm
		CPEI_EXITPROCESS,		// �v���Z�X�I���ʒm
		CPEI_GETSERVERIP,		// �T�[�oIP�擾
		CPEI_GETSERVERPORT,		// �T�[�o�|�[�g�擾
		CPEI_CONNECTSUCCESS,	// �ڑ�����
		CPEI_CONNECTFAILED,		// �ڑ����s
		CPEI_DISCONNECTED		// �ڑ��ؒf
	};
	
	CCasProxy(const HWND hProxyHwnd);
	~CCasProxy(void);

	const BOOL Connect(void);
	const DWORD TransmitCommand(const BYTE *pSendData, const DWORD dwSendSize, BYTE *pRecvData);

	static void SendEnterProcessEvent(const HWND hProxyHwnd);
	static void SendExitProcessEvent(const HWND hProxyHwnd);

protected:
	const DWORD SendProxyEvent(WPARAM wParam) const;

	const HWND m_hProxyHwnd;
	CSmartSock m_Socket;
	
	static DWORD dwErrorDelayTime;
};
