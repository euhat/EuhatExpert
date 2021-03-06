#include "stdafx.h"
#include "Expert.h"
#include "ExpertDlg.h"
#include <common/OpCommon.h>
#include <DbgHelp.h>

#define DEBUG_GDI_RESOURCES 0

#if DEBUG_GDI_RESOURCES
#include <apihook/APIHook.hpp>
#include <apihook/gdi/Gdi.hpp>
#endif
#include <EuhatPostDefMfc.h>

#pragma comment(lib, "dbghelp.lib")

BOOL IsDataSectionNeeded(const WCHAR *pModuleName)
{
	if (pModuleName == NULL)
		return FALSE;

	WCHAR szFileName[_MAX_FNAME] = L"";
	_wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);

	if (_wcsicmp(szFileName, L"ntdll") == 0)
		return TRUE;

	return FALSE;
}

BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
	if (pInput == NULL || pOutput == NULL)
		return FALSE;

	switch (pInput->CallbackType)
	{
	case ModuleCallback:
		if (pOutput->ModuleWriteFlags & ModuleWriteDataSeg)
			if (!IsDataSectionNeeded(pInput->Module.FullPath))
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);
	case IncludeModuleCallback:
	case IncludeThreadCallback:
	case ThreadCallback:
	case ThreadExCallback:
		return TRUE;
	}

	return FALSE;
}

void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = NULL;

		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
		mci.CallbackParam = 0;

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, pep != 0 ? &mdei : 0, NULL, &mci);

		CloseHandle(hFile);
	}
}

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	CreateMiniDump(pExceptionInfo, _T("EuhatExpert.dmp"));

	return EXCEPTION_EXECUTE_HANDLER;
}

void DisableSetUnhandledExceptionFilter()
{
	void *addr = (void *)GetProcAddress(LoadLibrary(_T("kernel32.dll")), "SetUnhandledExceptionFilter");

	if (addr)
	{
		unsigned char code[16];
		int size = 0;

		code[size++] = 0x33;
		code[size++] = 0xC0;
		code[size++] = 0xC2;
		code[size++] = 0x04;
		code[size++] = 0x00;

		DWORD dwOldFlag, dwTempFlag;
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
	}
}

void InitMinDump()
{
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

	//DisableSetUnhandledExceptionFilter();
}

BEGIN_MESSAGE_MAP(CExpertApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CExpertApp::CExpertApp()
{
//	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	
	InitMinDump();
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);

	char curPath[1024];
	GetModuleFileNameA(AfxGetApp()->m_hInstance, curPath, sizeof(curPath));
	string curDir = whPathGetContainer(curPath);
	SetCurrentDirectoryA(curDir.c_str());

#if DEBUG_GDI_RESOURCES
	apihook::StackWalker::Inst().Enable();
	apihook::gdi_base::EnableHook();

	apihook::gdi_pen::EnableHook();
	apihook::gdi_font::EnableHook();
	apihook::gdi_dc::EnableHook();
	apihook::gdi_bitmap::EnableHook();
	apihook::gdi_brush::EnableHook();
	apihook::gdi_extpen::EnableHook();
	apihook::gdi_palette::EnableHook();
	apihook::gdi_region::EnableHook();

	//apihook::gdi_pen::DisableHook();
#endif

#ifdef _DEBUG
	AllocConsole();
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "r+t", stdout);
#endif
}

CExpertApp theApp;

BOOL CExpertApp::InitInstance()
{
	EuhatBase euhatBase;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	HANDLE hSem = CreateSemaphore(NULL, 1, 1, m_pszExeName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		CloseHandle(hSem);
		HWND hWndPrevious = ::GetWindow(::GetDesktopWindow(), GW_CHILD);
		while (::IsWindow(hWndPrevious))
		{
			if (::GetProp(hWndPrevious, m_pszExeName))
			{
				::ShowWindow(hWndPrevious, SW_SHOW);
				::SetForegroundWindow(hWndPrevious);
				::SetForegroundWindow(::GetLastActivePopup(hWndPrevious));
				exit(0);
			}
			hWndPrevious = ::GetWindow(hWndPrevious, GW_HWNDNEXT);
		}
		exit(0);
	}

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	//SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	unique_ptr<CExpertDlg> dlg(new CExpertDlg(&euhatBase));
	dlg->app_ = this;
	dlg->Create(IDD_EXPERT_DIALOG, NULL);
	m_pMainWnd = dlg.get();
	MSG msg;
	int ret;
	while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (ret == -1)
			break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	m_pMainWnd = NULL;
	dlg.reset();

	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

#if DEBUG_GDI_RESOURCES
	apihook::gdi_base::MyStacks_base::Inst().Dump("gdi.leak");
	apihook::gdi_dc::MyStacks_relasedc::Inst().Dump("releasedc.leak");
	apihook::gdi_dc::MyStacks_deletedc::Inst().Dump("deletedc.leak");

	apihook::StackWalker::Inst().Disable();
#endif

	return FALSE;
}

