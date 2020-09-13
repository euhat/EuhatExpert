#include "stdafx.h"
#include "EuhatDamon.h"
#include <OpWin.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "base/HookDll.h"

static LRESULT CALLBACK monitorWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndTarget = (HWND)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (uMsg) {
	case WM_TIMER:
	{
		//if (NULL == FindWindow(RECORD_WIN_CLS_NAME, NULL))
		if (!IsWindow(hwndTarget))
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
	}
	case WM_CLOSE:
		KillTimer(hWnd, 1);
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#if 1
	AllocConsole();
	freopen("CONIN$", "r+t", stdin);
	freopen("CONOUT$", "r+t", stdout);
#endif

	string cmd = wstrToGb(lpCmdLine);
	vector<string> params;
	splitTokenString(cmd.c_str(), ' ', params);

	if (params[0] != "eu")
		return 0;

	HWND hwnd = (HWND)atoll(params[1].c_str());
	HookDll hookDll;
	hookDll.addVisitor(hwnd);
	hookDll.startHook();

	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASS wndCls;
	wndCls.cbClsExtra = 0;
	wndCls.cbWndExtra = 0;
	wndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndCls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndCls.hInstance = hInst;
	wndCls.lpfnWndProc = monitorWinProc;
	wndCls.lpszClassName = RECORD_WIN_CLS_NAME;
	wndCls.lpszMenuName = NULL;
	wndCls.style = CS_HREDRAW | CS_VREDRAW;

	ATOM nAtom = 0;
	nAtom = RegisterClass(&wndCls);
	if (nAtom == 0)
	{
		DBG(("register record monitor window failed.\n"));
		return 0;
	}

	HWND hwndMonitor;
	hwndMonitor = CreateWindow(RECORD_WIN_CLS_NAME, TEXT(""), WS_OVERLAPPEDWINDOW,
		0, 0, 0, 0, NULL, NULL, hInst, NULL);
	if (hwndMonitor == NULL)
	{
		DBG(("create record monitor window failed.\n"));
		return 0;
	}
	SetWindowLongPtr(hwndMonitor, GWLP_USERDATA, (LONG_PTR)hwnd);

	ShowWindow(hwndMonitor, SW_HIDE);
	UpdateWindow(hwndMonitor);

	SetTimer(hwndMonitor, 1, 1000, NULL);

	BOOL bRet;
	MSG msg;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	hookDll.stopHook();
	hookDll.removeVisitor(hwnd);

	return 1;
}
