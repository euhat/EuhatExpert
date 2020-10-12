#pragma once

#include <common/OpCommon.h>
#include <common/JyMutex.h>
#include <list>

using namespace std;

#define DWORD_PACK(_l,_r) (((DWORD)(_l)) << 16) | (((DWORD)(_r)) & 0xffff)
#define DWORD_UNPACK(_p,_l,_r) \
do { \
	_l = (WORD)(_p >> 16); \
	_r = (WORD)_p; \
} while (0)

#define WM_USER_MOUSE (WM_USER + 100)
#define WM_USER_KEYBOARD (WM_USER + 101)

#define RECORD_WIN_CLS_NAME L"_1EUHAT_WinMain"

#ifdef X64
#define RP_USE_HOOK_DLL 0
#else
#define RP_USE_HOOK_DLL	0
#endif

typedef BOOL (WINAPI *StartHookProc)(HINSTANCE hinst, HWND hwnd);
typedef void (WINAPI *StopHookProc)();

class HookDll
{
	HINSTANCE dll_;
	StartHookProc startApi_;
	StopHookProc stopApi_;

public:
	int loadHook();
	void freeHook();
	int startHook();
	int addVisitor(HWND hwnd);
	int removeVisitor(HWND hwnd);
	int stopHook();

	HHOOK hookMouseProc_;
	HHOOK hookKeyboardProc_;
	vector<HWND> hwnds_;
	WhMutex mutexHwnds_;
};