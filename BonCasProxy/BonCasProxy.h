// BonCasProxy.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��
#include "BonCasProxyDlg.h"


// CBonCasProxyApp:
// ���̃N���X�̎����ɂ��ẮABonCasProxy.cpp ���Q�Ƃ��Ă��������B
//

class CBonCasProxyApp : public CWinApp
{
public:
	CBonCasProxyApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����
	CBonCasProxyDlg *m_pBonCasProxyDlg;

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBonCasProxyApp theApp;