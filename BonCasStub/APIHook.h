// APIHook.h: APIフックモジュールのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(_APIHOOK_H_)
#define _APIHOOK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// APIフックモジュール 
void ReplaceApiProc(PCSTR pszModuleName, PROC pCurProc, PROC pNewProc);

#endif // !defined(_APIHOOK_H_)
