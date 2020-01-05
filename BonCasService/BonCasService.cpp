// BonCasService.cpp : WinMain �̎���


#include "stdafx.h"
#include "resource.h"
#include "BonCasService.h"
#include "CasServer.h"
#include "DnpService.h"
#include <stdio.h>


class CBonCasServiceModule : public CAtlServiceModuleT< CBonCasServiceModule, IDS_SERVICENAME >,
							 private CCasServer::ICasServerHandler
{
public :
	DECLARE_LIBID(LIBID_BonCasServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BONCASSERVICE, "{94799C2A-D5C7-4231-8F11-A3E492A14E5B}")

	CBonCasServiceModule()
		: m_CasServer(this)
		, m_wServerPort(6900U)
	{
		// �R���X�g���N�^
	}

	HRESULT InitializeSecurity() throw()
	{
		// TODO : CoInitializeSecurity ���Ăяo���A�T�[�r�X�ɓK�؂ȃZ�L�����e�B�ݒ�� 
		// �w�肵�܂��B
		// ���� - PKT ���x���̔F�؁A 
		// RPC_C_IMP_LEVEL_IDENTIFY �̋U�����x���A 
		// ����ѓK�؂� NULL �ȊO�̃Z�L�����e�B�L�q�q�B

		return S_OK;
	}

	//
	//	�R�}���h���C���̒ǉ�
	//
	//	�ȉ���3�̃R�}���h���C���I�v�V������ǉ�����B
	//
	//	xxx.exe /Start		�F�@�T�[�r�X�J�n
	//	xxx.exe /Stop		�F	�T�[�r�X��~
	//	xxx.exe /Restart	�F	�T�[�r�X�ċN��
	//
	bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
	{
		if(!__super::ParseCommandLine(lpCmdLine,pnRetCode))return false;

		bool ret;
		CString strName;
		CDnpService	cService;

		TCHAR szTokens[] = _T("-/");
		*pnRetCode = S_OK;

		LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);

		while(lpszToken != NULL)
		{
			if(WordCmpI(lpszToken, _T("Start")) == 0)
			{
				//�T�[�r�X�̊J�n
				strName.LoadString(IDS_SERVICENAME);
				ret = cService.EasyStart(strName);
				*pnRetCode = ret ? S_OK : E_FAIL;
				
				if(*pnRetCode == S_OK)::MessageBox(NULL, TEXT("�T�[�r�X�͐���ɊJ�n���܂���"), TEXT("BonCasService"), MB_OK);
				else ::MessageBox(NULL, TEXT("�T�[�r�X���o�^����Ă��܂���\r\n\r\n/Service �I�v�V�����œo�^���Ă�������"), TEXT("BonCasService"), MB_OK);
				
				return false;
			}

			if(WordCmpI(lpszToken, _T("Stop")) == 0)
			{
				//�T�[�r�X�̒�~
				strName.LoadString(IDS_SERVICENAME);
				ret = cService.EasyStop(strName);
				*pnRetCode = ret ? S_OK : E_FAIL;
				
				if(*pnRetCode == S_OK)::MessageBox(NULL, TEXT("�T�[�r�X�͐���ɒ�~���܂���"), TEXT("BonCasService"), MB_OK);
				else ::MessageBox(NULL, TEXT("�T�[�r�X���o�^����Ă��܂���\r\n\r\n/Service �I�v�V�����œo�^���Ă�������"), TEXT("BonCasService"), MB_OK);
				
				return false;
			}

			if(WordCmpI(lpszToken, _T("Restart")) == 0)
			{
				//�T�[�r�X�̍ċN��
				strName.LoadString(IDS_SERVICENAME);
				ret = cService.EasyRestart(strName);
				*pnRetCode = ret ? S_OK : E_FAIL;
				
				if(*pnRetCode == S_OK)::MessageBox(NULL, TEXT("�T�[�r�X�͐���ɍċN�����܂���"), TEXT("BonCasService"), MB_OK);
				else ::MessageBox(NULL, TEXT("�T�[�r�X���o�^����Ă��܂���\r\n\r\n/Service �I�v�V�����œo�^���Ă�������"), TEXT("BonCasService"), MB_OK);
				
				return false;
			}

			lpszToken = FindOneOf(lpszToken, szTokens);
		}

		return true;
	}

	HRESULT PreMessageLoop(int nShowCmd)
	{
		HRESULT hr = __super::PreMessageLoop(nShowCmd);

		#if _ATL_VER >= 0x0700
			if(SUCCEEDED(hr) && !m_bDelayShutdown)
			{
				hr = CoResumeClassObjects();
			}
		#endif

		if(SUCCEEDED(hr))
		{
			// �ݒ�t�@�C���̃p�X�����\�z����
			TCHAR szDrive[_MAX_DRIVE + 1] = {TEXT('\0')};
			TCHAR szDir[_MAX_DIR + 1] = {TEXT('\0')};
			TCHAR szFile[_MAX_FNAME + 1] = {TEXT('\0')};
		
			::GetModuleFileName(NULL, m_szConfigPath, sizeof(m_szConfigPath) - 1);
			::_tsplitpath(m_szConfigPath, szDrive, szDir, szFile, NULL);
			::_tmakepath(m_szConfigPath, szDrive, szDir, szFile, TEXT("ini"));

			m_wServerPort = (WORD)::GetPrivateProfileInt(TEXT("General"), TEXT("ServerPort"), m_wServerPort, m_szConfigPath);

			// �����ɃT�[�r�X�̏������ɕK�v�ȏ����������܂�
			m_CasServer.OpenServer(m_wServerPort);
		}

		return hr;
	}

	HRESULT PostMessageLoop(void)
	{
		// �����ɃT�[�r�X�I�����ɕK�v�Ȍ㏈���������܂��B
		m_CasServer.CloseServer();
		
		// �ݒ�ۑ�
		TCHAR szValue[1024];
		::_stprintf(szValue, TEXT("%u"), m_wServerPort);
		::WritePrivateProfileString(TEXT("General"), TEXT("ServerPort"), szValue, m_szConfigPath);

		return __super::PostMessageLoop();
	}

protected:
	virtual void OnCasServerEvent(CCasServer *pCasServer, const WORD wEventID)
	{
	
	}

	CCasServer m_CasServer;
	WORD m_wServerPort;
	TCHAR m_szConfigPath[_MAX_PATH];
};

CBonCasServiceModule _AtlModule;




extern "C" int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
	// ���������[�N���o�L��
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | ::_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)); 
#endif
 
    return _AtlModule.WinMain(nShowCmd);
}

