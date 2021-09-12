#include <EuhatPreDef.h>
#include "HookDll.h"
#include <EuhatPostDef.h>

#define HOOK_DLL_PATH			L"msg.dll"
#define HOOK_DLL_START_HOOK		"Start"
#define HOOK_DLL_STOP_HOOK		"Stop"

HookDll *gHookDll = NULL;

int HookDll::loadHook()
{
#if RP_USE_HOOK_DLL
	dll_ = LoadLibrary(HOOK_DLL_PATH);
	if (dll_ == NULL)
		return 0;

	startApi_ = (StartHookProc)GetProcAddress(dll_, HOOK_DLL_START_HOOK);
	if (startApi_ == NULL)
		goto failOut;

	stopApi_ = (StopHookProc)GetProcAddress(dll_, HOOK_DLL_STOP_HOOK);
	if (stopApi_ == NULL)
		goto failOut;

	return 1;

failOut:
	FreeLibrary(dll_);
	return 0;
#else
	dll_ = NULL;
	return 1;
#endif
}

void HookDll::freeHook()
{
#if RP_USE_HOOK_DLL
	FreeLibrary(dll_);
#endif
}

LRESULT WINAPI ll_record_mouse_proc(int ncode, WPARAM wparam, LPARAM lparam)
{
	LPMSLLHOOKSTRUCT p;
	WPARAM workerWparam;
	LPARAM workerLparam;

	p = (MSLLHOOKSTRUCT FAR *) lparam;

//	DBG(("hook mouse wparam: %x.", wparam));
	if (ncode >= 0 && p != NULL) {
		if (WM_MOUSEWHEEL == wparam) {
//			WLOG (("hook mouse wheel."));
			workerLparam = p->mouseData;
		} else {
			workerLparam = DWORD_PACK(p->pt.x, p->pt.y);
		}
		workerWparam = wparam;

//		WhMutexGuard guard(&gHookDll->mutexHwnds_);
		for (vector<HWND>::iterator it = gHookDll->hwnds_.begin(); it != gHookDll->hwnds_.end(); it++) {
			PostMessage(*it, WM_USER_MOUSE, workerWparam, workerLparam);
		}
	}

	return CallNextHookEx((HHOOK)gHookDll->hookMouseProc_, ncode, wparam, lparam);
}

LRESULT WINAPI ll_record_keyboard_proc(int ncode, WPARAM wparam, LPARAM lparam)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lparam;
	WPARAM workerWparam;
	LPARAM workerLparam;

	if (HC_ACTION == ncode && p != NULL)
	{
		workerWparam = wparam;
		workerLparam = DWORD_PACK(p->vkCode, p->scanCode);

//		WhMutexGuard guard(&gHookDll->mutexHwnds_);
		for (vector<HWND>::iterator it = gHookDll->hwnds_.begin(); it != gHookDll->hwnds_.end(); it++)
		{
			HWND hwnd = *it;
			PostMessage(hwnd, WM_USER_KEYBOARD, workerWparam, workerLparam);
		}
	}

	return CallNextHookEx(gHookDll->hookKeyboardProc_, ncode, wparam, lparam);
}

#ifdef _DEBUG
#define HOOK_DLL_DISABLE 0
#else
#define HOOK_DLL_DISABLE 0
#endif

int HookDll::startHook()
{
	gHookDll = this;

#if HOOK_DLL_DISABLE
	return 1;
#endif
	DBG(("enable hook dll.\n"));

	hookMouseProc_ = SetWindowsHookEx(WH_MOUSE_LL, ll_record_mouse_proc, dll_, 0);
	hookKeyboardProc_ = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)ll_record_keyboard_proc, dll_, 0);
	
	DBG(("hook keyboard proc 0x%p\n", hookKeyboardProc_));

#if RP_USE_HOOK_DLL
	//return startApi_(dll_, NULL);
	return 1;
#else
	return 1;
#endif
}

int HookDll::addVisitor(HWND hwnd)
{
	WhMutexGuard guard(&mutexHwnds_);
	hwnds_.push_back(hwnd);
	return 1;
}

int HookDll::removeVisitor(HWND hwnd)
{
	WhMutexGuard guard(&mutexHwnds_);
	for (vector<HWND>::iterator it = hwnds_.begin(); it != hwnds_.end(); it++) {
		if ((*it) == hwnd) {
			hwnds_.erase(it);
			return 1;
		}
	}
	return 0;
}

int HookDll::stopHook()
{
#if HOOK_DLL_DISABLE
	return 1;
#endif

	UnhookWindowsHookEx(hookMouseProc_);
	UnhookWindowsHookEx(hookKeyboardProc_);
	gHookDll = NULL;

#if RP_USE_HOOK_DLL
	stopApi_();
#endif
	return 1;
}
