// stdafx.cpp : �W���C���N���[�h BonNetCard.pch �݂̂�
// �܂ރ\�[�X �t�@�C���́A�v���R���p�C���ς݃w�b�_�[�ɂȂ�܂��B
// stdafx.obj �ɂ̓v���R���p�C���ς݌^��񂪊܂܂�܂��B

#include "stdafx.h"

// TODO: ���̃t�@�C���ł͂Ȃ��ASTDAFX.H �ŕK�v��
// �ǉ��w�b�_�[���Q�Ƃ��Ă��������B


#ifdef _DEBUG
	void DebugTrace(LPCTSTR szFormat, ...)
	{
		va_list Args;
		TCHAR szTempStr[1024 * 16];

		va_start(Args , szFormat);
		wvsprintf(szTempStr, szFormat, Args);
		va_end(Args);

		::OutputDebugString(szTempStr);
	}
#endif
