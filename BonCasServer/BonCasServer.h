// BonCasServer.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��
#include "BonCasServerDlg.h"


// CBonCasServerApp:
// ���̃N���X�̎����ɂ��ẮABonCasServer.cpp ���Q�Ƃ��Ă��������B
//

class CBonCasServerApp : public CWinApp
{
public:
	CBonCasServerApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����
	CBonCasServerDlg *m_pBonCasServerDlg;

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBonCasServerApp theApp;