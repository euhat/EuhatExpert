#pragma once
#include "EuhatChildWnd.h"

class EuhatScrollBar : public EuhatChildWnd
{
public:
	class User
	{
	public:
		virtual void onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset) = 0;
	};

private:
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void onLBtnDown(WPARAM wParam, int x, int y);
	void onMouseMove(WPARAM wParam, int x, int y);
	void onLBtnUp(WPARAM wParam, int x, int y);
	void onDraw(HDC hdc);
	void setBarPos(double offset);
	void freshAll();
	void moveInnerWindow();

	double outerLen_;
	double innerLen_;
	int innerWindowOffset_;

	double barOffset_;
	double barLen_;
	double mouseOffsetToBar_;

	int barCaught_;
	int barHighlighted_;

	User *user_;

protected:
	virtual void ui2logical(int &x, int &y) = 0;
	virtual void logical2ui(EuhatRect &rc, int startLen, int endLen) = 0;
	virtual int getLogicalLen(EuhatRect &rc) = 0;
	virtual void drawLogicalLine(HDC hdc, int lenStart, int lenVStart, int lenEnd, int lenVEnd) = 0;

public:
	EuhatScrollBar(HWND parentHwnd, User *user);
	~EuhatScrollBar();

	void setRange(int innerLen, int outerLen);
	void setPos(int innerWindowOffset);
	int getPos();

	unique_ptr<EuhatBrush> brushScroll_;
	unique_ptr<EuhatBrush> brushBar_;
	unique_ptr<EuhatBrush> brushBarHighlighted_;
	unique_ptr<EuhatBrush> brushBarCaught_;

	unique_ptr<EuhatPen> penBar_;
};