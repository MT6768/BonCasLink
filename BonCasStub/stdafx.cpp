// stdafx.cpp : 標準インクルード BonNetCard.pch のみを
// 含むソース ファイルは、プリコンパイル済みヘッダーになります。
// stdafx.obj にはプリコンパイル済み型情報が含まれます。

#include "stdafx.h"

// TODO: このファイルではなく、STDAFX.H で必要な
// 追加ヘッダーを参照してください。


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
