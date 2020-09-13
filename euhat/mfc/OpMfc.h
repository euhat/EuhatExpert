#pragma once

#include <common/OpCommon.h>

#define WH_USER_MSG_ADD_MSG (WM_USER + 101)

LRESULT whEditCtrlAddMsg(CEdit &editOutput, WPARAM wParam, LPARAM lParam);
void whEditCtrlMsg(HWND hwnd, const char *txt);

int whBrowsePath(HWND hwnd, CString &cstr);