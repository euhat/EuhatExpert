#include <EuhatPreDef.h>
#include <Windows.h>
#include "EuhatScrollBar.h"
#include <EuhatOsType.h>
#include <EuhatPostDef.h>

using namespace euhatwin;

EuhatScrollBar::EuhatScrollBar(HWND parentHwnd, EuhatScrollBar::User *user)
	: EuhatChildWnd(parentHwnd)
{
	user_ = user;
	barOffset_ = 0;
	innerWindowOffset_ = 0;

	brushScroll_.reset(new EuhatBrush(RGB(250, 250, 250)));
	brushBar_.reset(new EuhatBrush(GetSysColor(COLOR_BTNFACE)));
	brushBarHighlighted_.reset(new EuhatBrush(RGB(190, 190, 190)));
	brushBarCaught_.reset(new EuhatBrush(RGB(170, 170, 170)));

	penBar_.reset(new EuhatPen(RGB(200, 200, 200)));
}

EuhatScrollBar::~EuhatScrollBar()
{

}

LRESULT EuhatScrollBar::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
		return 0;
	}
	}
	return EuhatChildWnd::handleMsg(uMsg, wParam, lParam);
}

void EuhatScrollBar::onLBtnDown(WPARAM wParam, int x, int y)
{
	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);
	if (!rc.isIn(x, y))
	{
		ReleaseCapture();
		return;
	}

	ui2logical(x, y);

	if (x < barOffset_)
	{
		setBarPos(barOffset_ - barLen_);
	}
	else if (x > barOffset_ + barLen_)
	{
		setBarPos(barOffset_ + barLen_);
	}
	else
	{
		barCaught_ = 1;
		mouseOffsetToBar_ = x - barOffset_;
		SetCapture(hwnd_);
	}
}

void EuhatScrollBar::onMouseMove(WPARAM wParam, int x, int y)
{
	RECT rc;
	GetClientRect(hwnd_, &rc);

	if (barCaught_)
	{
		ui2logical(x, y);
		double offset = x - mouseOffsetToBar_;
		setBarPos(offset);
	}
	else
	{
		EuhatRect rcBar = rc;
		logical2ui(rcBar, (int)barOffset_, (int)(barOffset_ + barLen_));

		if (!rcBar.isIn(x, y))
		{
			if (barHighlighted_)
			{
				ReleaseCapture();
				barHighlighted_ = 0;
				InvalidateRect(hwnd_, NULL, 0);
			}
		}
		else
		{
			if (!barHighlighted_)
			{
				SetCapture(hwnd_);
				barHighlighted_ = 1;
				InvalidateRect(hwnd_, NULL, 0);
			}
		}
	}
}

void EuhatScrollBar::onLBtnUp(WPARAM wParam, int x, int y)
{
	if (barCaught_)
	{
		ReleaseCapture();
		barCaught_ = 0;
		barHighlighted_ = 0;
		InvalidateRect(hwnd_, NULL, 0);
	}
}

void EuhatScrollBar::onDraw(HDC hdc)
{
	RECT rc;
	GetClientRect(hwnd_, &rc);
	brushScroll_->FillRect(hdc, rc);

	EuhatRect rcBar = rc;
	logical2ui(rcBar, (int)barOffset_, (int)(barOffset_ + barLen_));

	if (barHighlighted_)
		brushBarHighlighted_->FillRect(hdc, rcBar.rc_);
	else if (barCaught_)
		brushBarCaught_->FillRect(hdc, rcBar.rc_);
	else
		brushBar_->FillRect(hdc, rcBar.rc_);

	int mx = (rcBar.rc_.left + rcBar.rc_.right) >> 1;
	int my = (rcBar.rc_.top + rcBar.rc_.bottom) >> 1;

	ui2logical(mx, my);

	EuhatGuard<EuhatPen> penGuard(penBar_.get(), hdc);
	drawLogicalLine(hdc, mx - 2, my - 4, mx - 2, my + 4);
	drawLogicalLine(hdc, mx + 2, my - 4, mx + 2, my + 4);
	drawRect(hdc, rcBar.rc_);
}

/*
 *  |----------------<outerLen>-----------|
 *  <                innerLen             >
 *  |----<bar>-----|
 *  <  scrollWin   >
 * so,
 *   bar.len      scrollWin.len     bar.offset      
 *   ---------- = --------------- = --------------- 
 *   outerLen     innerLen          innerWin.offset 
 * but we can customize bar.len, for it maybe very smaller due to above formula, so above formula will be invalid.
 * then we use below formula to calculate the relationship of bar middle offset and outer middle offset finally.
 *   scrollWin.len - bar.len    bar.middleOffset    bar.offset
 *   ------------------------ = ----------------- = ------------------
 *   innerLen - outerLen        outerMiddleOffset   innerWindowOffset
 */
void EuhatScrollBar::setRange(int innerLen, int outerLen)
{
	innerLen_ = innerLen == 0 ? 1 : innerLen;
	outerLen_ = outerLen == 0 ? 1 : outerLen;

	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);

	mouseOffsetToBar_ = 0;

	int rcLen = getLogicalLen(rc);
	barLen_ = 1.0 * rcLen * outerLen_ / innerLen_;
	if (barLen_ > rcLen)
		barLen_ = rcLen;
	if (barLen_ < EUHAT_SCROLL_BAR_MIN_LEN)
		barLen_ = EUHAT_SCROLL_BAR_MIN_LEN;

	setPos(innerWindowOffset_);

	barCaught_ = 0;
	barHighlighted_ = 0;

	moveInnerWindow();

	InvalidateRect(hwnd_, NULL, 0);
}

void EuhatScrollBar::freshAll()
{
	moveInnerWindow();

	::InvalidateRect(hwnd_, NULL, 0);
	::UpdateWindow(hwnd_);
}

void EuhatScrollBar::setBarPos(double offset)
{
	double oldOffset = barOffset_;

	barOffset_ = offset;

	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);
	int rcLen = getLogicalLen(rc);
	if (barOffset_ + barLen_ > rcLen)
		barOffset_ = rcLen - barLen_;
	if (barOffset_ < 0)
		barOffset_ = 0;

	if (oldOffset != barOffset_)
		freshAll();
}

void EuhatScrollBar::moveInnerWindow()
{
	user_->onScrollBarChange(this, getPos());
}

void EuhatScrollBar::setPos(int innerWindowOffset)
{
	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);
	double rcLen = getLogicalLen(rc);

	double outerMiddleOffset = innerWindowOffset;
	double barMiddleOffset = innerLen_ <= outerLen_ ? 0 : outerMiddleOffset * (rcLen - barLen_) / (innerLen_ - outerLen_);

	setBarPos(barMiddleOffset);
	innerWindowOffset_ = innerWindowOffset;
}

int EuhatScrollBar::getPos()
{
	EuhatRect rc;
	GetClientRect(hwnd_, &rc.rc_);
	double rcLen = getLogicalLen(rc);

	double barMiddleOffset = barOffset_;
	double outerMiddleOffset = rcLen <= barLen_? 0 : barMiddleOffset * (innerLen_ - outerLen_) / (rcLen - barLen_);
	innerWindowOffset_ = (int)outerMiddleOffset;
	return innerWindowOffset_;
}