#pragma once

#include <EuhatChildWnd.h>

class EuhatToolTip : public EuhatChildWnd
{
public:
	EuhatToolTip(HWND parentHwnd, const wchar_t *txt);
	~EuhatToolTip();

	void onDraw(HDC hdc);
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

	wstring txt_;

	unique_ptr<EuhatFont> font_;
	unique_ptr<EuhatPen> pen_;
	unique_ptr<EuhatBrush> brush_;

	HWND parentHwnd_;
};