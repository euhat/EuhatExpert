#include "stdafx.h"
#include "WorkTraceChart.h"
#include <EuhatPostDefMfc.h>

WorkTraceChart::WorkTraceChart(HWND parentHwnd, EuhatBase *euhatBase)
	: payload_(euhatBase),
	  EuhatBackWnd(parentHwnd, EUHAT_CHART_WIDTH + 64, EUHAT_CHART_X_AXIS_HEIGHT + EUHAT_CHART_MARK_HEIGHT)
{
	penNoRunning_.reset(new EuhatPen(RGB(255, 100, 100)));
	penNoMove_.reset(new EuhatPen(RGB(200, 200, 200)));
	penHangUp_.reset(new EuhatPen(RGB(255, 100, 255)));
	penMouse_.reset(new EuhatPen(RGB(0, 255, 0)));
	penKeyboard_.reset(new EuhatPen(RGB(64, 64, 255)));
	penScaler_.reset(new EuhatPen(RGB(0, 0, 0)));
}

WorkTraceChart::~WorkTraceChart()
{
}

LRESULT WorkTraceChart::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND: {
		return 0;
	}
	}
	return EuhatBackWnd::handleMsg(uMsg, wParam, lParam);
}

void WorkTraceChart::onMouseMove(WPARAM wParam, int x, int y)
{
	x += startX_;
	x_ = x;
	y_ = y;
	InvalidateRect(hwnd_, NULL, 0);
}

void WorkTraceChart::onDraw(HDC hdc)
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = memWidth_;
	rect.bottom = memHeight_;

	FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

	for (int i = 0; i < EUHAT_CHART_WIDTH; i++) {
		if (payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE) < 0 && payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) < 0) {
			if (payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE) <= EUHAT_WT_PL_NO_RECORD && payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) <= EUHAT_WT_PL_NO_RECORD) {
				penNoRunning_->drawLine(hdc, i, EUHAT_CHART_X_AXIS_HEIGHT, i, EUHAT_CHART_X_AXIS_HEIGHT + 15);
			} else if (payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE) == EUHAT_WT_PL_HANG_UP && payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) == EUHAT_WT_PL_HANG_UP) {
				penHangUp_->drawLine(hdc, i, EUHAT_CHART_X_AXIS_HEIGHT, i, EUHAT_CHART_X_AXIS_HEIGHT + 15);
			} else {
				penNoMove_->drawLine(hdc, i, EUHAT_CHART_X_AXIS_HEIGHT, i, EUHAT_CHART_X_AXIS_HEIGHT + 15);
			}
		}
	}

	int maxPayload = 1;
	for (int i = 0; i < EUHAT_CHART_WIDTH; i++) {
		int tmpValue = 0;
		if (payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE) >= 0) {
			tmpValue += payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE);
		}
		if (payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) >= 0) {
			tmpValue += payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) * 2;
		}
		if (maxPayload < tmpValue) {
			maxPayload = tmpValue;
		}
	}

	for (int i = 0; i < EUHAT_CHART_WIDTH; i++) {
		int mouseLen = 0;
		if (payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE) > 0) {
			mouseLen = (int)(EUHAT_CHART_X_AXIS_HEIGHT * (((double)payload_.getValue(i, EUHAT_WT_PL_TYPE_MOUSE)) / maxPayload));
			penMouse_->drawLine(hdc, i, EUHAT_CHART_X_AXIS_HEIGHT - mouseLen, i, EUHAT_CHART_X_AXIS_HEIGHT);
		}
		if (payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) > 0) {
			int keyboardLen = (int)(EUHAT_CHART_X_AXIS_HEIGHT * (((double)payload_.getValue(i, EUHAT_WT_PL_TYPE_KEYBOARD) * 2) / maxPayload));
			penKeyboard_->drawLine(hdc, i, EUHAT_CHART_X_AXIS_HEIGHT - mouseLen - keyboardLen, i, EUHAT_CHART_X_AXIS_HEIGHT - mouseLen);
		}
	}

	{
		EuhatGuard<EuhatPen> penGuard(penScaler_.get(), hdc);
		euhatwin::drawLine(hdc, 0, EUHAT_CHART_X_AXIS_HEIGHT, memWidth_, EUHAT_CHART_X_AXIS_HEIGHT);

		for (int i = 0; i <= 24; i++) {
			euhatwin::drawLine(hdc, i * 60 + 30, EUHAT_CHART_X_AXIS_HEIGHT, i * 60 + 30, EUHAT_CHART_X_AXIS_HEIGHT + 10);
			euhatwin::drawLine(hdc, i * 60, EUHAT_CHART_X_AXIS_HEIGHT, i * 60, EUHAT_CHART_X_AXIS_HEIGHT + 20);
			if (i < 24) {
				string intStr = intToString(i);
				CString dispStr = CA2T(intStr.c_str());
				TextOut(hdc, i * 60, EUHAT_CHART_X_AXIS_HEIGHT + 20, dispStr, dispStr.GetLength());
			}
		}
		euhatwin::drawLine(hdc, x_, 0, x_, EUHAT_CHART_X_AXIS_HEIGHT);
	}

	CString dispStr;
	dispStr.Format(_T("%02d:%02d"), x_ / 60, x_ % 60);
	TextOut(hdc, x_ + 10, 10, dispStr, dispStr.GetLength());
}

int WorkTraceChart::read(const char *filePath)
{
	int ret = payload_.read(filePath);
	if (!ret) {
		payload_.reset();
	}
	InvalidateRect(hwnd_, NULL, 0);
	return ret;
}