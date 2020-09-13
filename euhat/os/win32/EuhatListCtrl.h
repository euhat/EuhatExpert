#pragma once
#include "EuhatBackWnd.h"
#include <EuhatScrollVBar.h>
#include <EuhatIcon.h>
#include <EuhatToolTip.h>

#define EUHAT_LIST_CTRL_ATTR_CHECKBOX						0x1
#define EUHAT_LIST_CTRL_ATTR_MULTILINE						(0x1 << 1)
#define EUHAT_LIST_CTRL_ATTR_WHOLE_LINE						(0x1 << 2)

#define EUHAT_LIST_CTRL_CHECKBOX_WIDTH		16
#define EUHAT_LIST_CTRL_CHECKBOX_HEIGHT		16
#define EUHAT_LIST_CTRL_ICON_WIDTH			16
#define EUHAT_LIST_CTRL_ICON_HEIGHT			16

class EuhatListCtrl final : public EuhatBackWnd, public EuhatScrollBar::User
{
public:
	class User
	{
	public:
		virtual void onListCtrlDbClick(EuhatListCtrl *listCtrl, WPARAM wParam, int x, int y) {};
	};

	class Column
	{
	public:
		enum Type
		{
			TypeText,
			TypeInteger,
			TypeDateTime
		};
		Column(const wchar_t *name_, Type type, int width);
		int width_;
		Type type_;
		wstring name_;
		int isAscending_;
	};
	class ColumnRow
	{
	public:
		ColumnRow() : isSelected_(0), height_(0) {}
		int height_;
		int isSelected_;
		list<Column> cols_;
	};
	class Cell
	{
	public:
		Cell(const wchar_t *value) : value_(opStrDup(value)) {}
		Cell(int64_t i)	: i_(i) {}
		unique_ptr<wchar_t[]> value_;
		int64_t i_;
	};
	class Row
	{
	public:
		Row() { isSelected_ = 0; icon_ = NULL; iconSelected_ = NULL; }
		int height_;
		int isSelected_;
		HICON icon_;
		HICON iconSelected_;
		list<Cell> cells_;
	};
	class Runner
	{
		void initRect(EuhatRect &rc, int width, int height, POINT *curPos = NULL);
	public:
		virtual int onColumn(EuhatRect &rc, Column *col, ColumnRow &colRow) = 0;
		virtual int onCell(EuhatRect &rc, Cell *cell, Column *col, Row &row) = 0;
		void runColumns();
		void runRows();

		EuhatListCtrl *this_;
		POINT curPos_;
		EuhatRect winRc_;
	};
	class DrawRunner : public Runner
	{
		unique_ptr<EuhatIcon> iconCheckOn_;
		unique_ptr<EuhatIcon> iconCheckOff_;
	public:
		DrawRunner(EuhatListCtrl *pThis);
		void drawTextInRect(HDC hdc, const wstring &str, RECT &rc);
		void drawIconInRect(HDC hdc, HICON icon, RECT &rc, int width = EUHAT_LIST_CTRL_ICON_WIDTH, int height = EUHAT_LIST_CTRL_ICON_HEIGHT);
		void getDrawText(wstring &txt, Cell *cell, Column *col);
		int onColumn(EuhatRect &rc, Column *col, ColumnRow &colRow);
		int onCell(EuhatRect &rc, Cell *cell, Column *col, Row &row);
		void run();
		HDC hdc_;
	};
	class EventRunner : public Runner
	{
	public:
		enum MouseEvent
		{
			MouseLBtnDown,
			MouseLBtnUp,
			MouseMoving
		};
		EventRunner(EuhatListCtrl *pThis);
		void onEvent(WPARAM wParam, int x, int y, MouseEvent mouseEvent);

		int onColumn(EuhatRect &rc, Column *col, ColumnRow &colRow);
		int onCell(EuhatRect &rc, Cell *cell, Column *col, Row &row);
		void setAllSelected(int selected);

		WPARAM wParam_;
		EuhatRect selectRc_;
		MouseEvent mouseEvent_;
		int isMouseDown_;
		int needRefresh_;
		int isInSelectCheckBoxMode_;
		int isInSelectColumnMode_;
		Column *col_;
		Row *row_;
		Cell *cell_;
		unique_ptr<EuhatToolTip> toolTip_;
	};
private:

	User *user_;

public:
	LRESULT handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void onLBtnDown(WPARAM wParam, int x, int y);
	void onMouseMove(WPARAM wParam, int x, int y);
	void onLBtnUp(WPARAM wParam, int x, int y);
	void onDbClick(WPARAM wParam, int x, int y);
	void onMouseWheel(int zDelta, int fwKeys, int xPos, int yPos);
	void onDraw(HDC hdc);

	void onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset);

	EuhatListCtrl(HWND parentHwnd, User *user);
	~EuhatListCtrl();

	int getLogicalWidth();
	int getLogicalHeight();
	Row *getFirstSelectedRow();
	void correctPos(RECT &rc);

	ColumnRow cols_;
	list<Row> rows_;
	int attr_;

	COLORREF kColumnText;
	COLORREF kCellText;
	COLORREF kCellSelText;

	unique_ptr<EuhatFont> font_;
	unique_ptr<EuhatPen> penColumnLine_;
	unique_ptr<EuhatPen> penCellLine_;
	unique_ptr<EuhatBrush> brushColumnBack_;
	unique_ptr<EuhatBrush> brushCellBack_;
	unique_ptr<EuhatBrush> brushCellSelBack_;
	unique_ptr<EuhatBrush> brushNoneDataArea_;

	unique_ptr<DrawRunner> drawRunner_;
	unique_ptr<EventRunner> eventRunner_;

	unique_ptr<EuhatScrollVBar> vscroller_;
};