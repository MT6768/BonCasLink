// ConfigData.cpp: ConfigData クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// 定数定義
//////////////////////////////////////////////////////////////////////

// セクションの定義
static const TCHAR SectionArray[][256] =
{
	TEXT("General")		// CONFSECT_GENERAL
};


//////////////////////////////////////////////////////////////////////
// CConfigFile のインプリメンテーション
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigFile::CConfigFile()
{
	// 設定ファイルのパス名を構築する
	TCHAR szPath[_MAX_PATH + 1] = {TEXT('\0')};
	TCHAR szDrive[_MAX_DRIVE + 1] = {TEXT('\0')};
	TCHAR szDir[_MAX_DIR + 1] = {TEXT('\0')};
	TCHAR szFile[_MAX_FNAME + 1] = {TEXT('\0')};

	::GetModuleFileName(NULL, szPath, sizeof(szPath) - 1);
	::_tsplitpath(szPath, szDrive, szDir, szFile, NULL);
	::_tmakepath(m_szConfigPath, szDrive, szDir, szFile, TEXT("ini"));
}

CConfigData::CConfigFile::~CConfigFile()
{

}

LPCTSTR CConfigData::CConfigFile::GetPath(void)
{
	// 設定ファイルのパスを返す
	return m_szConfigPath;	
}

LPCTSTR CConfigData::CConfigFile::GetSection(const UINT nSection)
{
	// セクション名を返す
	return SectionArray[nSection];
}


//////////////////////////////////////////////////////////////////////
// CConfigData のインプリメンテーション
//////////////////////////////////////////////////////////////////////

CConfigData::CConfigFile CConfigData::m_ConfigFile;


//////////////////////////////////////////////////////////////////////
// CConfigBool のインプリメンテーション
//////////////////////////////////////////////////////////////////////

CConfigBool::CConfigBool()
	: m_bDefault(FALSE)
	, m_bValue(FALSE)
{

}

CConfigBool::CConfigBool(const UINT nSection, LPCTSTR lpszKeyName, const BOOL bDefault)
	: m_bDefault(FALSE)
	, m_bValue(FALSE)
{
	RegisterKey(nSection, lpszKeyName, bDefault);
}

void CConfigBool::RegisterKey(const UINT nSection, LPCTSTR lpszKeyName, const BOOL bDefault)
{
	// キー情報保存
	m_csKeyName = lpszKeyName;
	m_nSection = nSection;

	// データ情報保存
	m_bDefault = bDefault;

	// データ読み出し
	Load();
}

CConfigBool::~CConfigBool()
{
	// データ保存
	Save();
}

void CConfigBool::Load(void)
{
	// データロード
	m_bValue = (::GetPrivateProfileInt(m_ConfigFile.GetSection(m_nSection), m_csKeyName, (m_bDefault)? 1 : 0, m_ConfigFile.GetPath()) == 0)? FALSE : TRUE;
}

void CConfigBool::Save(void)
{
	// データセーブ
	::WritePrivateProfileString(m_ConfigFile.GetSection(m_nSection), m_csKeyName, (m_bValue)? TEXT("1") : TEXT("0"), m_ConfigFile.GetPath());
}

CConfigBool::operator BOOL () const
{
	// キャスト演算子
	return m_bValue;
}

CConfigBool::operator BOOL & ()
{
	// キャスト演算子
	return m_bValue;
}

CConfigBool::operator BOOL * ()
{
	// キャスト演算子
	return &m_bValue;
}

const BOOL & CConfigBool::operator = (const BOOL bValue)
{
	// 代入演算子
	return (m_bValue = bValue);
}


//////////////////////////////////////////////////////////////////////
// CConfigDword のインプリメンテーション
//////////////////////////////////////////////////////////////////////

CConfigDword::CConfigDword()
	: m_dwDefault(0UL)
	, m_dwRangeMax(0xFFFFFFFFUL)
	, m_dwRangeMin(0UL)
	, m_dwValue(0UL)
{

}

CConfigDword::CConfigDword(const UINT nSection, LPCTSTR lpszKeyName, const DWORD dwDefault, const DWORD dwRangeMax, const DWORD dwRangeMin)
	: m_dwDefault(0UL)
	, m_dwRangeMax(0xFFFFFFFFUL)
	, m_dwRangeMin(0UL)
	, m_dwValue(0UL)
{
	RegisterKey(nSection, lpszKeyName, dwDefault, dwRangeMax, dwRangeMin);
}

CConfigDword::~CConfigDword()
{
	// データ保存
	Save();
}

void CConfigDword::RegisterKey(const UINT nSection, LPCTSTR lpszKeyName, const DWORD dwDefault, const DWORD dwRangeMax, const DWORD dwRangeMin)
{
	// キー情報保存
	m_csKeyName = lpszKeyName;
	m_nSection = nSection;

	// データ情報保存
	m_dwDefault = dwDefault;
	m_dwRangeMax = dwRangeMax;
	m_dwRangeMin = dwRangeMin;

	// データ読み出し
	Load();
}

void CConfigDword::Load(void)
{
	// データロード
	DWORD dwValue = ::GetPrivateProfileInt(m_ConfigFile.GetSection(m_nSection), m_csKeyName, m_dwDefault, m_ConfigFile.GetPath());

	// クリップ
	if(dwValue > m_dwRangeMax)dwValue = m_dwRangeMax;
	if(dwValue < m_dwRangeMin)dwValue = m_dwRangeMin;

	m_dwValue = dwValue;
}

void CConfigDword::Save(void)
{
	// データセーブ
	DWORD dwValue = m_dwValue;

	// クリップ
	if(dwValue > m_dwRangeMax)dwValue = m_dwRangeMax;
	if(dwValue < m_dwRangeMin)dwValue = m_dwRangeMin;

	CString csValue;
	csValue.Format(TEXT("%lu"), dwValue);

	::WritePrivateProfileString(m_ConfigFile.GetSection(m_nSection), m_csKeyName, csValue, m_ConfigFile.GetPath());
}

CConfigDword::operator DWORD () const
{
	// キャスト演算子
	return m_dwValue;
}

CConfigDword::operator DWORD & ()
{
	// キャスト演算子
	return m_dwValue;
}

CConfigDword::operator DWORD * ()
{
	// キャスト演算子
	return &m_dwValue;
}

const DWORD & CConfigDword::operator = (const DWORD dwValue)
{
	// 代入演算子
	return (m_dwValue = dwValue);
}


//////////////////////////////////////////////////////////////////////
// CConfigString のインプリメンテーション
//////////////////////////////////////////////////////////////////////

CConfigString::CConfigString()
	: CString()
	, m_dwLengthMax(1024UL)
{

}

CConfigString::CConfigString(const UINT nSection, LPCTSTR lpszKeyName, LPCTSTR lpszDefault, const DWORD dwLengthMax)
	: CString()
	, m_dwLengthMax(1024UL)
{
	RegisterKey(nSection, lpszKeyName, lpszDefault, dwLengthMax);
}

CConfigString::~CConfigString()
{
	// データ保存
	Save();
}

void CConfigString::RegisterKey(const UINT nSection, LPCTSTR lpszKeyName, LPCTSTR lpszDefault, const DWORD dwLengthMax)
{
	// キー情報保存
	m_csKeyName = lpszKeyName;
	m_nSection = nSection;

	// データ情報保存
	m_csDefault = (lpszDefault)?lpszDefault : TEXT("");
	m_dwLengthMax = dwLengthMax;

	// データ読み出し
	Load();
}

void CConfigString::Load(void)
{
	// データロード
	DWORD dwLength = ::GetPrivateProfileString(m_ConfigFile.GetSection(m_nSection), m_csKeyName, m_csDefault, GetBuffer(m_dwLengthMax + 1), m_dwLengthMax, m_ConfigFile.GetPath());
	ReleaseBuffer();

	if(!dwLength || !GetLength()){
		*this = m_csDefault;
		}
}

void CConfigString::Save(void)
{
	// データセーブ
	*this = Left(m_dwLengthMax);
	::WritePrivateProfileString(m_ConfigFile.GetSection(m_nSection), m_csKeyName, (CString &)*this, m_ConfigFile.GetPath());
}

const CString& CConfigString::operator = (const CString& stringSrc)
{
	// 代入演算子
	(CString &)*this = stringSrc;
	return (CString &)*this;
}

const CString& CConfigString::operator = (TCHAR ch)
{
	// 代入演算子
	(CString &)*this = ch;
	return (CString &)*this;
}

const CString& CConfigString::operator = (const unsigned char* psz)
{
	// 代入演算子
	(CString &)*this = psz;
	return (CString &)*this;
}

const CString& CConfigString::operator = (LPCWSTR lpsz)
{
	// 代入演算子
	(CString &)*this = lpsz;
	return (CString &)*this;
}

const CString& CConfigString::operator = (LPCSTR lpsz)
{
	// 代入演算子
	(CString &)*this = lpsz;
	return (CString &)*this;
}
