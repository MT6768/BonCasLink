// BonCasService.cpp : WinMain の実装


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
		// コンストラクタ
	}

	HRESULT InitializeSecurity() throw()
	{
		// TODO : CoInitializeSecurity を呼び出し、サービスに適切なセキュリティ設定を 
		// 指定します。
		// 推奨 - PKT レベルの認証、 
		// RPC_C_IMP_LEVEL_IDENTIFY の偽装レベル、 
		// および適切な NULL 以外のセキュリティ記述子。

		return S_OK;
	}

	//
	//	コマンドラインの追加
	//
	//	以下の3つのコマンドラインオプションを追加する。
	//
	//	xxx.exe /Start		：　サービス開始
	//	xxx.exe /Stop		：	サービス停止
	//	xxx.exe /Restart	：	サービス再起動
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
				//サービスの開始
				strName.LoadString(IDS_SERVICENAME);
				ret = cService.EasyStart(strName);
				*pnRetCode = ret ? S_OK : E_FAIL;
				
				if(*pnRetCode == S_OK)::MessageBox(NULL, TEXT("サービスは正常に開始しました"), TEXT("BonCasService"), MB_OK);
				else ::MessageBox(NULL, TEXT("サービスが登録されていません\r\n\r\n/Service オプションで登録してください"), TEXT("BonCasService"), MB_OK);
				
				return false;
			}

			if(WordCmpI(lpszToken, _T("Stop")) == 0)
			{
				//サービスの停止
				strName.LoadString(IDS_SERVICENAME);
				ret = cService.EasyStop(strName);
				*pnRetCode = ret ? S_OK : E_FAIL;
				
				if(*pnRetCode == S_OK)::MessageBox(NULL, TEXT("サービスは正常に停止しました"), TEXT("BonCasService"), MB_OK);
				else ::MessageBox(NULL, TEXT("サービスが登録されていません\r\n\r\n/Service オプションで登録してください"), TEXT("BonCasService"), MB_OK);
				
				return false;
			}

			if(WordCmpI(lpszToken, _T("Restart")) == 0)
			{
				//サービスの再起動
				strName.LoadString(IDS_SERVICENAME);
				ret = cService.EasyRestart(strName);
				*pnRetCode = ret ? S_OK : E_FAIL;
				
				if(*pnRetCode == S_OK)::MessageBox(NULL, TEXT("サービスは正常に再起動しました"), TEXT("BonCasService"), MB_OK);
				else ::MessageBox(NULL, TEXT("サービスが登録されていません\r\n\r\n/Service オプションで登録してください"), TEXT("BonCasService"), MB_OK);
				
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
			// 設定ファイルのパス名を構築する
			TCHAR szDrive[_MAX_DRIVE + 1] = {TEXT('\0')};
			TCHAR szDir[_MAX_DIR + 1] = {TEXT('\0')};
			TCHAR szFile[_MAX_FNAME + 1] = {TEXT('\0')};
		
			::GetModuleFileName(NULL, m_szConfigPath, sizeof(m_szConfigPath) - 1);
			::_tsplitpath(m_szConfigPath, szDrive, szDir, szFile, NULL);
			::_tmakepath(m_szConfigPath, szDrive, szDir, szFile, TEXT("ini"));

			m_wServerPort = (WORD)::GetPrivateProfileInt(TEXT("General"), TEXT("ServerPort"), m_wServerPort, m_szConfigPath);

			// ここにサービスの初期化に必要な処理を書きます
			m_CasServer.OpenServer(m_wServerPort);
		}

		// サービスのステータスを明示的に RUNNING（実行中）に設定
		SetServiceStatus(SERVICE_RUNNING);

		return hr;
	}

	HRESULT PostMessageLoop(void)
	{
		// ここにサービス終了時に必要な後処理を書きます。
		m_CasServer.CloseServer();
		
		// 設定保存
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
	// メモリリーク検出有効
	::_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | ::_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)); 
#endif
 
    return _AtlModule.WinMain(nShowCmd);
}

