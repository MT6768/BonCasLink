// APIHook.cpp: API�t�b�N���W���[���̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "APIHook.h"
#include <windowsx.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <Dbghelp.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment(lib, "Dbghelp.lib")

//////////////////////////////////////////////////////////////////////
// �C���v�������e�[�V����
//////////////////////////////////////////////////////////////////////

// �ЂƂ̃��W���[���ɑ΂���API�t�b�N���s���֐�
static void ReplaceApiProcOne(PCSTR pszModuleName, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller)
{
	ULONG ulSize = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	if(!pImportDesc)return;

	while(pImportDesc->Name){
		PSTR pszModName = (PSTR)((BYTE *)hmodCaller + pImportDesc->Name);
		if(!lstrcmpiA(pszModName, pszModuleName))break;
        pImportDesc++;
		}

	if(!pImportDesc->Name)return;

	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((PBYTE) hmodCaller + pImportDesc->FirstThunk);

	while(pThunk->u1.Function){
		PROC *ppfn = (PROC *)&pThunk->u1.Function;
		if(*ppfn == pfnCurrent){
			DWORD dwDummy;
			VirtualProtect(ppfn, sizeof(ppfn), PAGE_EXECUTE_READWRITE, &dwDummy);
			WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
			return;
			}

		pThunk++;
		}

	return;
}


// ���ׂẴ��W���[���ɑ΂���API�t�b�N���s���֐�
void ReplaceApiProc(PCSTR pszModuleName, PROC pCurProc, PROC pNewProc) 
{
    // ���W���[�����X�g���擾
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	if(hModuleSnap == INVALID_HANDLE_VALUE)return;

	MODULEENTRY32 ModuleEntry;
	ModuleEntry.dwSize = sizeof(ModuleEntry);

	BOOL bModuleResult = Module32First(hModuleSnap, &ModuleEntry);
 
	// ���ꂼ��̃��W���[���ɑ΂���ReplaceIATEntryInOneMod�����s
	while(bModuleResult){        
		ReplaceApiProcOne(pszModuleName, pCurProc, pNewProc, ModuleEntry.hModule);
		bModuleResult = Module32Next(hModuleSnap, &ModuleEntry);
		}

	CloseHandle(hModuleSnap);
}
