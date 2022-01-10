#include <EuhatPreDef.h>
#include <Windows.h>
#include <Windowsx.h>
#include "EuhatToolTip.h"
#include <EuhatPostDef.h>

EuhatToolTip::EuhatToolTip(HWND parentHwnd, const wchar_t *txt)
	: EuhatChildWnd(parentHwnd)
{
	long style;
	style = GetWindowLong(hwnd_, GWL_STYLE);
	style &= ~WS_CHILD;
	style |= WS_POPUP;
	SetWindowLong(hwnd_, GWL_STYLE, style);
	long exStyle;
	exStyle = GetWindowLong(hwnd_, GWL_EXSTYLE);
	exStyle &= ~WS_EX_APPWINDOW;
	exStyle |= WS_EX_TOOLWINDOW;
	SetWindowLong(hwnd_, GWL_EXSTYLE, exStyle);

	SetParent(hwnd_, NULL);
	parentHwnd_ = parentHwnd;

	txt_ = txt;

	font_.reset(new EuhatFont(L"MS Shell Dlg", 15));
	pen_.reset(new EuhatPen(RGB(100, 100, 100)));
	brush_.reset(new EuhatBrush(RGB(255, 255, 255)));
}

EuhatToolTip::~EuhatToolTip()
{

}

void EuhatToolTip::onDraw(HDC hdc)
{
	int marginX = 3;
	EuhatGuard<EuhatFont> fontGuard(font_.get(), hdc);

	EuhatRect rc;
	GetWindowRect(hwnd_, &rc.rc_);

	SIZE size;
	GetTextExtentPoint32(hdc, txt_.c_str(), (int)txt_.size(), &size);

	if (rc.width() != size.cx + 2 * marginX)
	{
		rc.setWidthHeight(size.cx + 2 * marginX, rc.height());
		rc.move(-marginX, 0);
		int cx = GetSystemMetrics(SM_CXFULLSCREEN);
		if (rc.rc_.right > cx)
			rc.move(cx - rc.rc_.right, 0);
		moveWindow(&rc.rc_);
	}

	rc.move(-rc.rc_.left, -rc.rc_.top);

	brush_->FillRect(hdc, rc.rc_);

	TextOut(hdc, marginX, (rc.height() - size.cy) / 2, txt_.c_str(), (int)txt_.length());

	pen_->drawRect(hdc, rc.rc_);
}

LRESULT EuhatToolTip::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ClientToScreen(hwnd_, &pt);
		ScreenToClient(parentHwnd_, &pt);
		lParam = MAKELPARAM(pt.x, pt.y);
		::PostMessage(parentHwnd_, uMsg, wParam, lParam);
		break;
	}
	}
	return EuhatChildWnd::handleMsg(uMsg, wParam, lParam);
}