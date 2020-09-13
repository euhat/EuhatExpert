#pragma once
#include "EuhatChildWnd.h"

#define EUHAT_BUTTON_SKIN_PATH "skin"

class EuhatBitmap;
class EuhatButton;

class EuhatButtonUser
{
public:
	virtual void onBtnClick(EuhatButton *btn) = 0;
};

class EuhatButton : public EuhatChildWnd
{
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void onLBtnDown(WPARAM wParam, int x, int y);
	void onMouseMove(WPARAM wParam, int x, int y);
	void onLBtnUp(WPARAM wParam, int x, int y);
	void onDraw(HDC hdc);

	EuhatButtonUser *user_;
	EuhatBitmap *bmpNormal_;
	EuhatBitmap *bmpHighlighted_;
	EuhatBitmap *bmpDown_;
	EuhatBitmap *bmpToDraw_;

public:
	EuhatButton(HWND parentHwnd, EuhatButtonUser *user);
	~EuhatButton();

	void setSkin(const char *folder, const char *name);
};