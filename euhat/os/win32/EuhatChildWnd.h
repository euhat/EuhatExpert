#pragma once

#include <common/OpCommon.h>

namespace euhatwin
{
	void drawLine(HDC hdc, int startX, int startY, int endX, int endY);
	void drawRect(HDC hdc, RECT &rc);
}

class EuhatRect
{
public:
	EuhatRect();
	EuhatRect(RECT &rc);
	EuhatRect(int left, int top, int right, int bottom);

	int isIn(int x, int y);
	void move(int offsetX, int offsetY);
	void grow(int widthStep, int heightStep);
	void positive();
	int isIntersect(RECT &rc);

	int width();
	int height();
	void setWidthHeight(int width, int height);

	RECT rc_;
};

class EuhatFont
{
	int height_;
public:
	EuhatFont(const wchar_t *fontName, int fontHeight, int fontWidth = 0);
	~EuhatFont();

	int getHeight() { return height_; }

	HFONT handle_;
};

class EuhatPen
{
public:
	EuhatPen(COLORREF rgb, int thick = 1, int type = PS_SOLID);
	~EuhatPen();

	void drawLine(HDC hdc, int startX, int startY, int endX, int endY);
	void drawRect(HDC hdc, RECT &rc);

	HPEN handle_;
};

class EuhatBrush
{
public:
	EuhatBrush(COLORREF rgb);
	~EuhatBrush();

	void FillRect(HDC hdc, RECT &rc);

	HBRUSH handle_;
};

template<class T>
class EuhatGuard
{
	T *t_;
	HDC hdc_;
	HGDIOBJ original_;
public:
	EuhatGuard(T *t, HDC hdc)
		: t_(t), hdc_(hdc)
	{
		original_ = SelectObject(hdc_, t->handle_);
	}
	~EuhatGuard()
	{
		SelectObject(hdc_, original_);
	}
};

#define EUHAT_CHILD_WND_USE_PROP 1
#if EUHAT_CHILD_WND_USE_PROP
#define EUHAT_CHILD_WND_PROP_NAME _T("__EuhatChildWnd")
typedef HANDLE EuhatChildUserData;
#else
typedef LONG_PTR EuhatChildUserData;
#endif

class EuhatChildWnd
{
	static LRESULT CALLBACK euhatChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void registerClass(LPCTSTR lpszClass, HINSTANCE hInst);
	EuhatChildUserData getProp();
	void setProp(EuhatChildUserData useData);

protected:
	virtual LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void onLBtnDown(WPARAM wParam, int x, int y);
	virtual void onMouseMove(WPARAM wParam, int x, int y);
	virtual void onLBtnUp(WPARAM wParam, int x, int y);
	virtual void onDbClick(WPARAM wParam, int x, int y);
	virtual void onMouseWheel(int zDelta, int fwKeys, int xPos, int yPos);
	virtual void onDraw(HDC hdc);

	inline void logical2ui(POINT &pt) { pt.x -= startPos_.x; pt.y -= startPos_.y; }
	inline void ui2logical(POINT &pt) { pt.x += startPos_.x; pt.y += startPos_.y; }
	inline void logical2ui(EuhatRect &rc) { rc.move(-startPos_.x, -startPos_.y); }
	inline void ui2logical(EuhatRect &rc) {	rc.move(startPos_.x, startPos_.y); }

	WNDPROC parentWndProc_;
	HWND parentHwnd_;
	EuhatChildUserData parentUserData_;

	POINT startPos_;

public:
	EuhatChildWnd(HWND parentHwnd);
	virtual ~EuhatChildWnd();

	void moveWindow(RECT *rect);

	HWND hwnd_;
};