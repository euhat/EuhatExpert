#include "stdafx.h"
#include "WorkTrace.h"
#include "../../Expert.h"
#include <common/OpCommon.h>
#include "../../../EuhatDamon/base/HookDll.h"
#include <EuhatPostDefMfc.h>

WorkTrace::WorkTrace(EuhatBase *euhatBase)
	: payload_(euhatBase)
{
	euhatBase_ = euhatBase;
	threadHandle_ = WH_INVALID_THREAD;
}

WorkTrace::~WorkTrace()
{

}

void WorkTrace::payloadAddOne(int type)
{
	time_t t = time(NULL);
	tm *local = localtime(&t);
	int slot = local->tm_hour * 60 + local->tm_min;

	if (slot < lastSlot_) {
		updateNoMoveGap(EUHAT_WT_PAYLOAD_COUNT - 1);
		lastSlot_ = 0;

		time_t yeT = t - 24 * 60 * 60;
		tm *yeLocal = localtime(&yeT);
		char filePath[256];
		strftime(filePath, 254, EUHAT_WT_PL_FILE_PATH_PATTERN, yeLocal);
		payload_.write(filePath);

		payload_.reset();
	}

	if (type >= 0) {
		if (payload_.getValue(slot, type) < 0) {
			payload_.setValue(slot, type, 1);
		}
		payload_.setValue(slot, type, payload_.getValue(slot, type) + 1);
//		DBG(("slot %d, type %d, value %d.\n", slot, type, payload_.getValue(slot, type)));
	}

	if (lastSlot_ != slot) {
		updateNoMoveGap(slot);

		char filePath[256];
		local = localtime(&t);
		strftime(filePath, 254, EUHAT_WT_PL_FILE_PATH_PATTERN, local);
		payload_.write(filePath);
	}
}

void WorkTrace::updateNoMoveSlot(int slot, int value)
{
	if (payload_.getValue(slot, EUHAT_WT_PL_TYPE_MOUSE) == EUHAT_WT_PL_NO_RECORD) {
		payload_.setValue(slot, EUHAT_WT_PL_TYPE_MOUSE, value);
	}
	if (payload_.getValue(slot, EUHAT_WT_PL_TYPE_KEYBOARD) == EUHAT_WT_PL_NO_RECORD) {
		payload_.setValue(slot, EUHAT_WT_PL_TYPE_KEYBOARD, value);
	}
}

void WorkTrace::updateNoMoveGap(int curSlot)
{
	int fillValue = EUHAT_WT_PL_NO_MOVE;
	if (abs(lastSlot_ - curSlot) > 1)
		fillValue = EUHAT_WT_PL_HANG_UP;
	if (lastSlot_ >= 0) {
		if (lastSlot_ > curSlot) {
			for (int i = lastSlot_; i < EUHAT_WT_PAYLOAD_COUNT; i++) {
				updateNoMoveSlot(i, fillValue);
			}
			for (int i = 0; i < curSlot; i++) {
				updateNoMoveSlot(i, fillValue);
			}
		} else {
			for (int i = lastSlot_; i <= curSlot; i++) {
				updateNoMoveSlot(i, fillValue);
			}
		}
	}
	lastSlot_ = curSlot;
}

int WorkTrace::recordMouse(WPARAM wParam, LPARAM lParam)
{
	if (WM_MOUSEWHEEL == wParam) {
		int delta = (int)(lParam >> 16);
//		DBG(("mouse wheel %d.\n", delta));
	} else {
		POINT pt;
		DWORD_UNPACK(lParam, pt.x, pt.y);
//		HWND hwnd;
//		hwnd = WindowFromPointZOrder(pt);
//		DBG(("mouse %d, x:%d, y:%d.\n", (DWORD)wParam, pt.x, pt.y));
	}
	payloadAddOne(EUHAT_WT_PL_TYPE_MOUSE);
	return 1;
}

int WorkTrace::recordKeyboard(WPARAM wParam, LPARAM lParam)
{
	int vkCode;
	int scanCode;
	DWORD_UNPACK(lParam, vkCode, scanCode);
//	DBG(("keyboard wparam:%d, vk:%d, scan:%d.\n", (short)wParam, vkCode, scanCode));
 	payloadAddOne(EUHAT_WT_PL_TYPE_KEYBOARD);
	return 1;
}

static LRESULT CALLBACK hookWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WorkTrace *pThis = (WorkTrace *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (uMsg) {
	case WM_USER_MOUSE:
		pThis->recordMouse(wParam, lParam);
		break;
	case WM_USER_KEYBOARD:
		pThis->recordKeyboard(wParam, lParam);
		break;
	case WM_TIMER:
		pThis->payloadAddOne(-1);
		break;
	case WM_COMMAND:
/*		switch (LOWORD(wParam)) {
		case IDC_COPYDATA1:
		case IDC_COPYDATA2:
			break;
		}
*/
		break;
	case WM_CLOSE:
		KillTimer(hWnd, 1);
		pThis->euhatBase_->demonThread_->stop();
		DestroyWindow(hWnd);
		pThis->euhatBase_->demonThread_.reset();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

static void WINAPI recordThread(LPVOID pParam)
{
	WorkTrace *pThis = (WorkTrace *)pParam;

	HINSTANCE hInst = GetModuleHandle(0);

	WNDCLASS wndCls;
	wndCls.cbClsExtra = 0;
	wndCls.cbWndExtra = 0;
	wndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndCls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndCls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndCls.hInstance = hInst;
	wndCls.lpfnWndProc = hookWinProc;
	wndCls.lpszClassName = RECORD_WIN_CLS_NAME;
	wndCls.lpszMenuName = NULL;
	wndCls.style = CS_HREDRAW | CS_VREDRAW;

	ATOM nAtom = 0;
	nAtom = RegisterClass(&wndCls);
	if (nAtom == 0)
	{
		DBG(("register record window failed.\n"));
		return;
	}

	HWND hWnd;
	hWnd = CreateWindow(RECORD_WIN_CLS_NAME, TEXT(""), WS_OVERLAPPEDWINDOW,
		300, 300, 600, 200, NULL, NULL, hInst, NULL);
	if (hWnd == NULL)
	{
		DBG(("create record window failed.\n"));
		return;
	}
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	SetTimer(hWnd, 1, 1000 * 30, NULL);

	pThis->euhatBase_->demonThread_.reset(new EuhatDemonThread());
	pThis->euhatBase_->demonThread_->hwnd_ = hWnd;
	pThis->euhatBase_->demonThread_->start();
	pThis->euhatBase_->workTraceListeningHwnd_ = hWnd;

	BOOL bRet;
	MSG msg;
	while ((bRet = GetMessage(&msg, hWnd, 0, 0)) != 0)
	{
		if (bRet == -1)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClass(RECORD_WIN_CLS_NAME, hInst);
}

int WorkTrace::startRecord()
{
	if (NULL != threadHandle_)
		return 0;

	time_t t = time(NULL);
	char filePath[256];
	tm *local = localtime(&t);
	strftime(filePath, 254, EUHAT_WT_PL_FILE_PATH_PATTERN, local);
	DBG(("open payload file [%s].\n", filePath));

	payload_.read(filePath);
	lastSlot_ = -1;

	euhatBase_->workTraceListeningHwnd_ = NULL;

	DWORD threadId;
	threadHandle_ = CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)recordThread,
		(void *)this,
		0,
		&threadId);
	
	return 1;
}

int WorkTrace::stopRecord()
{
	if (NULL == threadHandle_)
		return 0;

	while (NULL == euhatBase_->workTraceListeningHwnd_)
		whSleep(1);

	PostMessage(euhatBase_->workTraceListeningHwnd_, WM_CLOSE, 0, 0);
	euhatBase_->workTraceListeningHwnd_ = NULL;

	WaitForSingleObject(threadHandle_, INFINITE);
	CloseHandle(threadHandle_);
	threadHandle_ = WH_INVALID_THREAD;

	return 1;
}