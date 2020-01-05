// BonCasServer.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "BonCasServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBonCasServerApp

BEGIN_MESSAGE_MAP(CBonCasServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBonCasServerApp �R���X�g���N�V����

CBonCasServerApp::CBonCasServerApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CBonCasServerApp �I�u�W�F�N�g�ł��B

CBonCasServerApp theApp;


// CBonCasServerApp ������

BOOL CBonCasServerApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	m_pBonCasServerDlg = new CBonCasServerDlg;
	m_pBonCasServerDlg->Create(CBonCasServerDlg::IDD);
	m_pMainWnd = m_pBonCasServerDlg;

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return TRUE;
}

int CBonCasServerApp::ExitInstance()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	delete m_pBonCasServerDlg;

	return CWinApp::ExitInstance();
}
