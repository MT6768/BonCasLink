// BonCasProxy.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "BonCasProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBonCasProxyApp

BEGIN_MESSAGE_MAP(CBonCasProxyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBonCasProxyApp �R���X�g���N�V����

CBonCasProxyApp::CBonCasProxyApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CBonCasProxyApp �I�u�W�F�N�g�ł��B

CBonCasProxyApp theApp;


// CBonCasProxyApp ������

BOOL CBonCasProxyApp::InitInstance()
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

	m_pBonCasProxyDlg = new CBonCasProxyDlg;
	m_pBonCasProxyDlg->Create(CBonCasProxyDlg::IDD);
	m_pMainWnd = m_pBonCasProxyDlg;

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return TRUE;
}

int CBonCasProxyApp::ExitInstance()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	delete m_pBonCasProxyDlg;

	return CWinApp::ExitInstance();
}
