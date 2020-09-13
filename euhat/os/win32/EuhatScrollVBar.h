#pragma once
#include "EuhatScrollBar.h"

class EuhatScrollVBar final : public EuhatScrollBar
{
protected:
	void ui2logical(int &x, int &y) { swap(x, y); }
	void logical2ui(EuhatRect &rc, int startLen, int endLen) { rc.rc_.top = startLen; rc.rc_.bottom = endLen; }
	int getLogicalLen(EuhatRect &rc) { return rc.height(); }
	void drawLogicalLine(HDC hdc, int lenStart, int lenVStart, int lenEnd, int lenVEnd) { euhatwin::drawLine(hdc, lenVStart, lenStart, lenVEnd, lenEnd); }
public:
	EuhatScrollVBar(HWND parentHwnd, User *user) : EuhatScrollBar(parentHwnd, user) {}
};