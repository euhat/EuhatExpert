#include <EuhatPreDef.h>
#include <Windows.h>
#include <Windowsx.h>
#include "EuhatTabCtrl.h"
#include <EuhatPostDef.h>

EuhatTabCtrl::EuhatTabCtrl(HWND parentHwnd, EuhatTabCtrl::Listener *listener)
	: EuhatChildWnd(parentHwnd)
{
	curSel_ = -1;
	curHi_ = -1;

	kUnselText_ = RGB(200, 200, 200);
	kSelText_ = RGB(0, 0, 0);

	brushBack_.reset(new EuhatBrush(RGB(100, 100, 100)));
	brushBackSel_.reset(new EuhatBrush(RGB(255, 255, 255)));
	penBack_.reset(new EuhatPen(RGB(155, 155, 155)));
	penBackSel_.reset(new EuhatPen(RGB(0, 0, 0)));
	font_.reset(new EuhatFont(L"MS Shell Dlg", 15));

	listener_ = listener;
	isMouseDown_ = 0;
}

EuhatTabCtrl::~EuhatTabCtrl()
{
}

LRESULT EuhatTabCtrl::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND: {
		return 0;
	}
	}
	return EuhatChildWnd::handleMsg(uMsg, wParam, lParam);
}

void EuhatTabCtrl::onMouseMove(WPARAM wParam, int x, int y)
{
	if (isMouseDown_)
		return;

	int curHi = inWhatTab(x, y);
	if (curHi >= 0)
	{
		SetCapture(hwnd_);
	}
	else
	{
		ReleaseCapture();
	}
	if (curHi != curHi_)
	{
		curHi_ = curHi;
		InvalidateRect(hwnd_, NULL, 0);
	}
}

void EuhatTabCtrl::onLBtnDown(WPARAM wParam, int x, int y)
{
	isMouseDown_ = 1;

	SetCapture(hwnd_);

	int curSel = inWhatTab(x, y);

	if (curSel != curSel_)
	{
		curSel_ = curSel;
		InvalidateRect(hwnd_, NULL, 0);
	}
}

void EuhatTabCtrl::onLBtnUp(WPARAM wParam, int mouseX, int mouseY)
{
	ReleaseCapture();

	isMouseDown_ = 0;

	curSel_ = inWhatTab(mouseX, mouseY);

	if (curSel_ >= 0 && NULL != listener_)
		listener_->onClick(*this);

	InvalidateRect(hwnd_, NULL, 0);
}

int EuhatTabCtrl::getCurSel()
{
	return curSel_;
}

void EuhatTabCtrl::setCurSel(int curSel)
{
	curSel_ = curSel;
	InvalidateRect(hwnd_, NULL, 0);
}

int EuhatTabCtrl::inWhatTab(int mouseX, int mouseY)
{
	RECT rc;
	GetClientRect(hwnd_, &rc);

	if (mouseY < rc.top || rc.bottom < mouseY)
		return -1;

	int x = 0;
	int i = 0;
	for (vector<EuhatTabInfo>::iterator it = tabs_.begin(); it != tabs_.end(); it++, i++)
	{
		if (x < mouseX && mouseX < x + it->width)
		{
			return i;
		}
		x += it->width;
	}
	return -1;
}

int EuhatTabCtrl::getWidth()
{
	int x = 0;
	for (vector<EuhatTabInfo>::iterator it = tabs_.begin(); it != tabs_.end(); it++)
	{
		x += it->width;
	}
	return x;
}

void EuhatTabCtrl::onDraw(HDC hdc)
{
	EuhatGuard<EuhatFont> fontGuard(font_.get(), hdc);

	size_t count = tabs_.size();
	if (count == 0)
		return;

	RECT rc;
	GetWindowRect(hwnd_, &rc);
	size_t width = (rc.right - rc.left) / count;

	SetBkMode(hdc, TRANSPARENT);

	int startY = (rc.bottom - rc.top - fontHeight_) / 2;
	int x = 0;
	int i = 0;
	for (vector<EuhatTabInfo>::iterator it = tabs_.begin(); it != tabs_.end(); it++, i++)
	{
		RECT rcTab;
		rcTab.left = x;
		x += it->width;
		rcTab.right = x;
		rcTab.top = 0;
		rcTab.bottom = rc.bottom - rc.top;

		if (curSel_ != i)
		{
			brushBack_->FillRect(hdc, rcTab);

			SetTextColor(hdc, kUnselText_);
			TextOutA(hdc, rcTab.left + 10, rcTab.top + startY, it->name.c_str(), (int)it->name.size());
		}
		else
		{
			brushBackSel_->FillRect(hdc, rcTab);

			SetTextColor(hdc, kSelText_);
			TextOutA(hdc, rcTab.left + 10, rcTab.top + startY, it->name.c_str(), (int)it->name.size());
		}

		if (curHi_ == i)
		{
			if (curSel_ != i)
			{
				penBack_->drawRect(hdc, rcTab);
			}
			else
			{
				penBackSel_->drawRect(hdc, rcTab);
			}
		}
	}
}

int EuhatTabCtrl::add(const char *name)
{
	EuhatTabInfo tabInfo;
	tabInfo.name = name;
	tabInfo.width = 0;

	HDC hdc = ::GetWindowDC(hwnd_);
	{
		EuhatGuard<EuhatFont> fontGuard(font_.get(), hdc);

		SIZE size;
		GetTextExtentPoint32A(hdc, tabInfo.name.c_str(), (int)tabInfo.name.size(), &size);
		tabInfo.width = size.cx + 20;
		fontHeight_ = size.cy;
	}
	::ReleaseDC(hwnd_, hdc);

	tabs_.push_back(tabInfo);

	return 1;
}
