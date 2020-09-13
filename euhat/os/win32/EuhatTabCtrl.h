#pragma once

#include "EuhatChildWnd.h"
#include <EuhatBitmap.h>

struct EuhatTabInfo
{
	string name;
	int width;
};

class EuhatTabCtrl : public EuhatChildWnd
{
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void onMouseMove(WPARAM wParam, int x, int y);
	void onLBtnDown(WPARAM wParam, int x, int y);
	void onLBtnUp(WPARAM wParam, int x, int y);
	void onDraw(HDC hdc);
	int inWhatTab(int x, int y);

	int curSel_;
	int curHi_;
	int isMouseDown_;
	vector<EuhatTabInfo> tabs_;
	int fontHeight_;

public:
	class Listener
	{
	public:
		virtual void onClick(EuhatTabCtrl &tabCtrl) = 0;
	};

	EuhatTabCtrl(HWND parentHwnd, Listener *listener = NULL);
	~EuhatTabCtrl();

	int add(const char *name);
	int getWidth();
	int getCurSel();
	void setCurSel(int curSel);

	EuhatBitmap *bmpBk_;

	COLORREF kUnselText_;
	COLORREF kSelText_;

	unique_ptr<EuhatBrush> brushBack_;
	unique_ptr<EuhatBrush> brushBackSel_;
	unique_ptr<EuhatPen> penBack_;
	unique_ptr<EuhatPen> penBackSel_;
	unique_ptr<EuhatFont> font_;

private:
	Listener *listener_;

};