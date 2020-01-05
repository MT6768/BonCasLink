// ConfigData.h: CConfigData �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////
// �Z�N�V�����̒�`
//////////////////////////////////////////////////////////////////////

#define CONFSECT_GENERAL		0UL


//////////////////////////////////////////////////////////////////////
// ���C���^�t�F�[�X(�ݒ�ۑ��p�ɕs�������̃f�[�^�^����������)
//////////////////////////////////////////////////////////////////////

class CConfigData  
{
public:
	virtual void Load(void) = 0;
	virtual void Save(void) = 0;

protected:
	// �ݒ�t�@�C���̃p�X�ƃZ�N�V�������Ǘ�����
	class CConfigFile
	{
	public:
		CConfigFile();
		virtual ~CConfigFile();

		LPCTSTR GetPath(void);
		LPCTSTR GetSection(const UINT nSection);

	protected:
		TCHAR m_szConfigPath[_MAX_PATH + 1];
	};

	static CConfigFile m_ConfigFile;

	// �A�C�e�����̏��(�Z�N�V�����A�L�[)
	CString m_csKeyName;
	UINT m_nSection;
};


//////////////////////////////////////////////////////////////////////
// �s������ BOOL �f�[�^�^
//////////////////////////////////////////////////////////////////////

class CConfigBool : public CConfigData
{
public:
	CConfigBool();
	CConfigBool(const UINT nSection, LPCTSTR lpszKeyName, const BOOL bDefault = FALSE);
	virtual ~CConfigBool();

	void RegisterKey(const UINT nSection, LPCTSTR lpszKeyName, const BOOL bDefault = FALSE);

	virtual void Load(void);
	virtual void Save(void);

	operator BOOL () const;
	operator BOOL & ();
	operator BOOL * ();
	const BOOL & operator = (const BOOL bValue);

private:
	BOOL m_bDefault;
	BOOL m_bValue;
};


//////////////////////////////////////////////////////////////////////
// �s������ DWORD �f�[�^�^
//////////////////////////////////////////////////////////////////////

class CConfigDword : public CConfigData
{
public:
	CConfigDword();
	CConfigDword(const UINT nSection, LPCTSTR lpszKeyName, const DWORD dwDefault = 0, const DWORD dwRangeMax = 0xFFFFFFFF, const DWORD dwRangeMin = 0);
	virtual ~CConfigDword();

	void RegisterKey(const UINT nSection, LPCTSTR lpszKeyName, const DWORD dwDefault = 0, const DWORD dwRangeMax = 0xFFFFFFFF, const DWORD dwRangeMin = 0);

	virtual void Load(void);
	virtual void Save(void);

	operator DWORD () const;
	operator DWORD & ();
	operator DWORD * ();
	const DWORD & operator = (const DWORD dwValue);

private:
	DWORD m_dwDefault;
	DWORD m_dwRangeMax;
	DWORD m_dwRangeMin;
	DWORD m_dwValue;
};


//////////////////////////////////////////////////////////////////////
// �s������ CString �f�[�^�^
//////////////////////////////////////////////////////////////////////

class CConfigString : public CConfigData, public CString
{
public:
	CConfigString();
	CConfigString(const UINT nSection, LPCTSTR lpszKeyName, LPCTSTR lpszDefault = NULL, const DWORD dwLengthMax = 1024);
	virtual ~CConfigString();

	void RegisterKey(const UINT nSection, LPCTSTR lpszKeyName, LPCTSTR lpszDefault = NULL, const DWORD dwLengthMax = 1024);

	virtual void Load(void);
	virtual void Save(void);

	const CString& operator = (const CString& stringSrc);
	const CString& operator = (TCHAR ch);
	const CString& operator = (const unsigned char* psz);
	const CString& operator = (LPCWSTR lpsz);
	const CString& operator = (LPCSTR lpsz);

private:
	CString m_csDefault;
	DWORD m_dwLengthMax;
};
