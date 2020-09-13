#pragma once
#include "EuhatScrollBar.h"

class EuhatScrollHBar final : public EuhatScrollBar
{
protected:
	void ui2logical(int &x, int &y) {}
	void logical2ui(EuhatRect &rc, int startLen, int endLen) { rc.rc_.left = startLen; rc.rc_.right = endLen; }
	int getLogicalLen(EuhatRect &rc) { return rc.width(); }
	void drawLogicalLine(HDC hdc, int lenStart, int lenVStart, int lenEnd, int lenVEnd) { euhatwin::drawLine(hdc, lenStart, lenVStart, lenEnd, lenVEnd); }
public:
	EuhatScrollHBar(HWND parentHwnd, User *user) : EuhatScrollBar(parentHwnd, user) {}
};