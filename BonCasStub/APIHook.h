// APIHook.h: API�t�b�N���W���[���̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(_APIHOOK_H_)
#define _APIHOOK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// API�t�b�N���W���[�� 
void ReplaceApiProc(PCSTR pszModuleName, PROC pCurProc, PROC pNewProc);

#endif // !defined(_APIHOOK_H_)
