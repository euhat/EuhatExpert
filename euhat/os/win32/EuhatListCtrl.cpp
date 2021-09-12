#include <EuhatPreDef.h>
#include <Windows.h>
#include "EuhatListCtrl.h"
#include <EuhatOsType.h>
#include <EuhatPostDef.h>

using namespace euhatwin;

EuhatListCtrl::Column::Column(const wchar_t *name, Type type, int width)
	: name_(name), type_(type), width_(width), isAscending_(1)
{

}

void EuhatListCtrl::Runner::initRect(EuhatRect &rc, int width, int height, POINT *curPos)
{
	if (NULL == curPos)
		curPos = &curPos_;
	rc.rc_.left = curPos->x;
	rc.rc_.top = curPos->y;
	rc.rc_.right = rc.rc_.left + width;
	curPos->x += width;
	rc.rc_.bottom = rc.rc_.top + height;
}

void EuhatListCtrl::Runner::runColumns()
{
	EuhatRect rc;
	POINT curPos;

	curPos.x = curPos.y = 0;
		
	for (list<Column>::iterator it = this_->cols_.cols_.begin(); it != this_->cols_.cols_.end(); it++)
	{
		Column *col = &*it;
		initRect(rc, col->width_, this_->cols_.height_, &curPos);
		if (!onColumn(rc, col, this_->cols_))
			return;
	}

	curPos_.x = curPos.x;
}

void EuhatListCtrl::Runner::runRows()
{
	winRc_.rc_.top += this_->cols_.height_;
	curPos_.y += this_->cols_.height_;

	EuhatRect rc;
	size_t colCount = this_->cols_.cols_.size();

	for (list<Row>::iterator it = this_->rows_.begin(); it != this_->rows_.end(); it++)
	{
		Row &row = *it;
		curPos_.x = -this_->startPos_.x;

		if (curPos_.y + row.height_ < winRc_.rc_.top)
		{
			curPos_.y += row.height_;
			continue;
		}

		list<Column>::iterator itCol = this_->cols_.cols_.begin();
		list<Cell>::iterator itCell = row.cells_.begin();
		for (; itCol != this_->cols_.cols_.end(); itCol++)
		{
			Column *col = &*itCol;
			initRect(rc, col->width_, row.height_);
			if (!onCell(rc, itCell != row.cells_.end() ? &*itCell++ : NULL, col, row))
				return;
		}

		curPos_.y += row.height_;
		if (curPos_.y > winRc_.rc_.bottom)
			break;
	}

}

EuhatListCtrl::EuhatListCtrl(HWND parentHwnd, User *user)
	: EuhatBackWnd(parentHwnd)
{
	kColumnText = RGB(0, 0, 0);
	kCellText = RGB(0, 0, 0);
	kCellSelText = RGB(255, 255, 255);

	brushColumnBack_.reset(new EuhatBrush(GetSysColor(COLOR_BTNFACE)));
	brushCellBack_.reset(new EuhatBrush(RGB(255, 255, 255)));
	brushCellSelBack_.reset(new EuhatBrush(RGB(0, 0, 150)));
	brushNoneDataArea_.reset(new EuhatBrush(RGB(200, 200, 200)));

	penColumnLine_.reset(new EuhatPen(RGB(200, 200, 200)));
	penCellLine_.reset(new EuhatPen(RGB(230, 230, 230)));

	attr_ = EUHAT_LIST_CTRL_ATTR_CHECKBOX | EUHAT_LIST_CTRL_ATTR_MULTILINE | EUHAT_LIST_CTRL_ATTR_WHOLE_LINE;

	font_.reset(new EuhatFont(L"MS Shell Dlg", 15));
	drawRunner_.reset(new DrawRunner(this));
	eventRunner_.reset(new EventRunner(this));

	user_ = user;
}

EuhatListCtrl::~EuhatListCtrl()
{
}

int EuhatListCtrl::getLogicalWidth()
{
	int i = 0;
	for (auto it = cols_.cols_.begin(); it != cols_.cols_.end(); it++)
	{
		i += it->width_;
	}
	return i;
}

int EuhatListCtrl::getLogicalHeight()
{
	int i = cols_.height_;
	for (auto it = rows_.begin(); it != rows_.end(); it++)
	{
		i += it->height_;
	}
	return i;
}

EuhatListCtrl::Row *EuhatListCtrl::getFirstSelectedRow()
{
	for (auto it = rows_.begin(); it != rows_.end(); it++)
	{
		if (it->isSelected_)
			return &*it;
	}
	return NULL;
}

void EuhatListCtrl::correctPos(RECT &rcList)
{
	rcList.right -= EUHAT_SCROLL_BAR_DEFAULT_WIDTH;

	int innerWidth = getLogicalWidth();
	EuhatListCtrl::Column &col = cols_.cols_.back();
	if (col.width_ == 0)
	{
		col.width_ = rcList.right - rcList.left - innerWidth + 1;
	}
	moveWindow(&rcList);

	RECT rcVScroll = rcList;
	rcVScroll.left = rcVScroll.right;
	rcVScroll.right += EUHAT_SCROLL_BAR_DEFAULT_WIDTH;
	vscroller_->moveWindow(&rcVScroll);
	vscroller_->setRange(getLogicalHeight(), rcList.bottom - rcList.top);
}

LRESULT EuhatListCtrl::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_COMMAND: {
		return 0;
	}
	case WM_KILLFOCUS: {
		eventRunner_->toolTip_.reset();
		return 0;
	}
	}
	return EuhatBackWnd::handleMsg(uMsg, wParam, lParam);
}

void EuhatListCtrl::onLBtnDown(WPARAM wParam, int x, int y)
{
	eventRunner_->onEvent(wParam, x, y, EventRunner::MouseLBtnDown);
}

void EuhatListCtrl::onMouseMove(WPARAM wParam, int x, int y)
{
	eventRunner_->onEvent(wParam, x, y, EventRunner::MouseMoving);
}

void EuhatListCtrl::onLBtnUp(WPARAM wParam, int x, int y)
{
	eventRunner_->onEvent(wParam, x, y, EventRunner::MouseLBtnUp);
}

void EuhatListCtrl::onDbClick(WPARAM wParam, int x, int y)
{
	if (y < cols_.height_)
		return;
	user_->onListCtrlDbClick(this, wParam, x, y);
}

void EuhatListCtrl::onMouseWheel(int zDelta, int fwKeys, int xPos, int yPos)
{
	if (NULL == vscroller_.get())
		return;
	int pos = vscroller_->getPos();
	vscroller_->setPos(pos - zDelta);
}

void EuhatListCtrl::onDraw(HDC hdc)
{
	drawRunner_->hdc_ = hdc;
	drawRunner_->run();
}

void EuhatListCtrl::onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset)
{
	startPos_.y = innerWindowOffset;
	InvalidateRect(hwnd_, NULL, 0);
}