// TsUtilClass.cpp: TS���[�e�B���e�B�[�N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TsUtilClass.h"


//////////////////////////////////////////////////////////////////////
// CDynamicReferenceable �N���X�̍\�z/����
//////////////////////////////////////////////////////////////////////

CDynamicReferenceable::CDynamicReferenceable()
	: m_dwRefCount(0UL)
{

}

CDynamicReferenceable::~CDynamicReferenceable()
{

}

void CDynamicReferenceable::AddRef(void)
{
	// �Q�ƃJ�E���g�C���N�������g
	m_dwRefCount++;
}

void CDynamicReferenceable::ReleaseRef(void)
{
	// �Q�ƃJ�E���g�f�N�������g
	if(m_dwRefCount){
		// �C���X�^���X�J��
		if(!(--m_dwRefCount))delete this;
		}
#ifdef _DEBUG
	else{
		::DebugBreak();
		}
#endif
}


//////////////////////////////////////////////////////////////////////
// CCriticalLock �N���X�̍\�z/����
//////////////////////////////////////////////////////////////////////

CCriticalLock::CCriticalLock()
{
	// �N���e�B�J���Z�N�V����������
	::InitializeCriticalSection(&m_CriticalSection);
}

CCriticalLock::~CCriticalLock()
{
	// �N���e�B�J���Z�N�V�����폜
	::DeleteCriticalSection(&m_CriticalSection);
}

void CCriticalLock::Lock(void)
{
	// �N���e�B�J���Z�N�V�����擾
	::EnterCriticalSection(&m_CriticalSection);
}

void CCriticalLock::Unlock(void)
{
	// �N���e�B�J���Z�N�V�����J��
	::LeaveCriticalSection(&m_CriticalSection);
}
	
	
//////////////////////////////////////////////////////////////////////
// CBlockLock �N���X�̍\�z/����
//////////////////////////////////////////////////////////////////////
	
CBlockLock::CBlockLock(CCriticalLock *pCriticalLock)
	: m_pCriticalLock(pCriticalLock)
{
	// ���b�N�擾
	m_pCriticalLock->Lock();
}

CBlockLock::~CBlockLock()
{
	// ���b�N�J��
	m_pCriticalLock->Unlock();
}
