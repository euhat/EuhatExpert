#include <EuhatPreDef.h>
#include <windows.h>
#include "EuhatBackWnd.h"
#include <EuhatPostDef.h>

LRESULT EuhatBackWnd::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps = { 0 };
		BeginPaint(hwnd_, &ps);
		HDC hdc = ps.hdc;
		if (NULL == memHdc_)
		{
			memHdc_ = CreateCompatibleDC(hdc);
			memBmp_ = CreateCompatibleBitmap(hdc, memWidth_, memHeight_);
			oldBmp_ = SelectObject(memHdc_, memBmp_);
			SetBkMode(memHdc_, TRANSPARENT);
		}

		onDraw(memHdc_);

		RECT rect;
		GetClientRect(hwnd_, &rect);
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, memHdc_, startX_, startY_, SRCCOPY);
		EndPaint(hwnd_, &ps);
		return 1;
	}
	case WM_SIZE:
	{
		if (!sameSizeAsFront_)
			break;
		resetMemDc();
		InvalidateRect(hwnd_, NULL, 0);
		break;
	}
	}
	return EuhatChildWnd::handleMsg(uMsg, wParam, lParam);
}

EuhatBackWnd::EuhatBackWnd(HWND parentHwnd, int width, int height)
	: EuhatChildWnd(parentHwnd)
{
	memHdc_ = NULL;
	oldBmp_ = NULL;
	if (width == 0 && height == 0)
	{
		sameSizeAsFront_ = 1;
		resetMemDc();
	}
	else
	{
		memWidth_ = width;
		memHeight_ = height;
		sameSizeAsFront_ = 0;
	}
	startX_ = 0;
	startY_ = 0;
}

EuhatBackWnd::~EuhatBackWnd()
{

}

void EuhatBackWnd::resetMemDc()
{
	if (sameSizeAsFront_)
	{
		EuhatRect rc;
		GetClientRect(hwnd_, &rc.rc_);
		memWidth_ = rc.width();
		memHeight_ = rc.height();
	}

	if (NULL == memHdc_)
		return;

	DeleteObject(memBmp_);
	SelectObject(memHdc_, oldBmp_);
	DeleteDC(memHdc_);

	memHdc_ = NULL;
}