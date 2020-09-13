#pragma once
#include "EuhatChildWnd.h"

class EuhatBackWnd : public EuhatChildWnd
{
	HDC memHdc_;
	HGDIOBJ oldBmp_;
	HBITMAP memBmp_;

protected:
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void resetMemDc();

	int memWidth_;
	int memHeight_;
	int sameSizeAsFront_;

public:
	EuhatBackWnd(HWND parentHwnd, int width = 0, int height = 0);
	virtual ~EuhatBackWnd();

	int startX_;
	int startY_;
};