#pragma once

#include "atlstr.h"

class	CDnpService
{
	//
	//	サービスの起動／停止用スレッドクラス
	//
	class CServiceThread
	{
	public:
		CServiceThread()
		{
			_bCancel = false;
		}

	private:

		bool					_bCancel;			//サービスの起動／停止処理中断用変数、trueなら中断開始
		CComAutoCriticalSection	_secbCancel;		//サービスの起動／停止処理中断用クリティカルセクション

	public:

		//
		//	サービスの起動／停止処理中断用関数
		//
		//	中断したい場合はIsCancel(true,true)を呼び出す
		//
		bool	IsCancel(bool bSave=false,bool bNewValue=false)
		{
			bool	ret;

			_secbCancel.Lock();
				if(bSave)
				{
					_bCancel = bNewValue;
					ret = true;
				}
				else
					ret = _bCancel;
			_secbCancel.Unlock();

			return	ret;
		}



		//
		//	サービスの簡易コントロール
		//
		//	そのまま呼び出すとサービスが起動／停止するまで無限ループで待機する。
		//	スレッド中で呼び出し、IsCancel()を利用することで無限ループに陥らない
		//	コントロールが可能。
		//
		bool	EasyStartStop(LPCTSTR pszName,bool bStart)
		{
			bool			ret;
			BOOL			bRet;
			SC_HANDLE		hManager;
			SC_HANDLE		hService;
			SERVICE_STATUS	sStatus;

			ret = false;
			hManager = NULL;
			hService = NULL;
			while(1)			//無限ループではない！
			{
				hManager = ::OpenSCManager(NULL,NULL,GENERIC_EXECUTE);
				if(hManager == NULL)
					break;

				if(bStart)
					hService = ::OpenService(hManager,pszName,SERVICE_START | SERVICE_QUERY_STATUS);
				else
					hService = ::OpenService(hManager,pszName,SERVICE_STOP | SERVICE_QUERY_STATUS);
				if(hService == NULL)
					break;

				::ZeroMemory(&sStatus,sizeof(SERVICE_STATUS));
				bRet = ::QueryServiceStatus(hService,&sStatus);
				if(bRet == FALSE)
					break;

				if(bStart && sStatus.dwCurrentState == SERVICE_RUNNING)
				{
					//既にサービスは動いている
					ret = true;
					break;
				}
				if((bStart == false) && sStatus.dwCurrentState == SERVICE_STOPPED)
				{
					//既にサービスは止まっている
					ret = true;
					break;
				}

				if(bStart)
				{
					////////////////////////////
					//	サービス開始
					//

					if(sStatus.dwCurrentState == SERVICE_STOPPED)
					{
						//サービス開始要求
						bRet = ::StartService(hService,NULL,NULL);
						if(bRet == FALSE)
							break;

						//開始まで無限ループで待機
						//IsCancelを利用すれば無限ループからの脱出が可能
						while(::QueryServiceStatus(hService,&sStatus))
						{
							if(sStatus.dwCurrentState != SERVICE_RUNNING)
							{
								ret = true;
								break;
							}

							if(IsCancel())
								break;

							//::Sleep(sStatus.dwWaitHint);
							//本来ならばdwWaitHitだけSleepをかけるが、中断処理を入れるため
							//500msecを上限にSleepする
							::Sleep((sStatus.dwWaitHint > 500) ? 500 : sStatus.dwWaitHint);
							continue;
						}
					}
					break;
				}


				////////////////////////////
				//	サービス停止
				//

				if(sStatus.dwCurrentState == SERVICE_RUNNING)
				{
					//サービス停止要求
					bRet = ::ControlService(hService,SERVICE_CONTROL_STOP,&sStatus);
					if(bRet == FALSE)
						break;

					//停止まで無限ループで待機
					//IsCancelを利用すれば無限ループからの脱出が可能
					while(::QueryServiceStatus(hService,&sStatus))
					{
						if(sStatus.dwCurrentState == SERVICE_STOPPED)
						{
							ret = true;
							break;
						}

						if(IsCancel())
							break;

						//::Sleep(sStatus.dwWaitHint);
						//本来ならばdwWaitHitだけSleepをかけるが、中断処理を入れるため
						//500msecを上限にSleepする
						::Sleep((sStatus.dwWaitHint > 500) ? 500 : sStatus.dwWaitHint);
						continue;
					}
				}

				break;		//必須！この行がないと無限ループになるかも
			}

			ATLASSERT(ret);

			if(hService)
				::CloseServiceHandle(hService);
			if(hManager)
				::CloseServiceHandle(hManager);

			return	ret;
		}
	};


public:


	//
	//	サービスの簡易コントロール
	//
	//	サービスが起動/停止するまで無限ループで待機する。
	//
	bool	EasyStartStop(LPCTSTR pszName,bool bStart)
	{
		CServiceThread	cThread;

		return	cThread.EasyStartStop(pszName,bStart);
	}


	//
	//	サービスの簡易起動
	//
	//	サービスが起動するまで無限ループで待機する。
	//
	bool	EasyStart(LPCTSTR pszName)
	{
		return	EasyStartStop(pszName,true);
	}

	//
	//	サービスの簡易停止
	//
	//	サービスが停止するまで無限ループで待機する。
	//
	bool	EasyStop(LPCTSTR pszName)
	{
		return	EasyStartStop(pszName,false);
	}


	//
	//	サービスの簡易再起動
	//
	//	サービスが再起動するまで無限ループで待機する。
	//
	bool	EasyRestart(LPCTSTR pszName)
	{
		bool			ret;
		CServiceThread	cThread;

		ret = cThread.EasyStartStop(pszName,false);
		if(ret)
			ret = cThread.EasyStartStop(pszName,true);

		return	ret;
	}



	//
	//	指定するサービスが動いているかのチェック
	//
	//	falseの場合は"停止"とは限らない。サービスが存在しない場合などもfalseとなる。
	//
	bool	IsServiceRunning(LPCTSTR pszName)
	{
		bool			ret;
		BOOL			bRet;
		SC_HANDLE		hManager;
		SC_HANDLE		hService;
		SERVICE_STATUS	sStatus;

		ret = false;
		hManager = NULL;
		hService = NULL;
		while(1)			//無限ループではない！
		{
			hManager = ::OpenSCManager(NULL,NULL,GENERIC_EXECUTE);
			ATLASSERT(hManager);
			if(hManager == NULL)
				break;

			hService = ::OpenService(hManager,pszName,SERVICE_QUERY_STATUS);
			ATLASSERT(hService);
			if(hService == NULL)
				break;

			::ZeroMemory(&sStatus,sizeof(SERVICE_STATUS));
			bRet = ::QueryServiceStatus(hService,&sStatus);
			ATLASSERT(bRet);
			if(bRet == FALSE)
				break;

			if(sStatus.dwCurrentState == SERVICE_RUNNING)
				ret = true;

			break;		//必須
		}

		if(hService)
			::CloseServiceHandle(hService);
		if(hManager)
			::CloseServiceHandle(hManager);

		return	ret;
	}
};
