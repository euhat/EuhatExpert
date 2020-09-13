#include <EuhatPreDef.h>
#include <Windows.h>
#include "EuhatListCtrl.h"
#include <EuhatPostDef.h>

using namespace euhatwin;

#define IDI_ICON_CHECK_ON               158
#define IDI_ICON_CHECK_OFF              157

EuhatListCtrl::DrawRunner::DrawRunner(EuhatListCtrl *pThis)
{
	this_ = pThis;
	iconCheckOn_.reset(new EuhatIcon(IDI_ICON_CHECK_ON));
	iconCheckOff_.reset(new EuhatIcon(IDI_ICON_CHECK_OFF));
}

void EuhatListCtrl::DrawRunner::run()
{
	EuhatGuard<EuhatFont> fontGuard(this_->font_.get(), hdc_);

/*	SIZE size;
	wstring str = L"M";
	GetTextExtentPoint32(hdc, str.c_str(), str.size(), &size);
*/
	SetBkMode(hdc_, TRANSPARENT);

	curPos_.x = curPos_.y = 0;
	this_->logical2ui(curPos_);
	GetClientRect(this_->hwnd_, &winRc_.rc_);

	runRows();
	runColumns();

	int goneWidth = curPos_.x;
	int goneHeight = curPos_.y;
	RECT bottomRc;
	bottomRc.top = goneHeight;
	bottomRc.bottom = winRc_.rc_.bottom;
	bottomRc.left = 0;
	bottomRc.right = goneWidth;
	this_->brushNoneDataArea_->FillRect(hdc_, bottomRc);
	RECT rightRc;
	rightRc.top = 0;
	rightRc.bottom = winRc_.rc_.bottom;
	rightRc.left = goneWidth;
	rightRc.right = winRc_.rc_.right;
	this_->brushNoneDataArea_->FillRect(hdc_, rightRc);

	EuhatGuard<EuhatPen> penGuard(this_->penColumnLine_.get(), hdc_);
	drawLine(hdc_, 0, 0, 0, goneHeight);
	drawLine(hdc_, 0, 0, goneWidth, 0);

	if (this_->eventRunner_->isMouseDown_)
	{
		EuhatRect rc;
		rc.rc_ = this_->eventRunner_->selectRc_.rc_;
		this_->logical2ui(rc);
		drawRect(hdc_, rc.rc_);
	}
}

void EuhatListCtrl::DrawRunner::drawTextInRect(HDC hdc, const wstring &str, RECT &rc)
{
	int rcHeight = rc.bottom - rc.top;
	if (rcHeight > this_->font_->getHeight())
	{
		RECT rcText = rc;
		int gap = (rcHeight - this_->font_->getHeight()) >> 1;
		rcText.top += gap;
		rcText.bottom -= gap;
		rcText.left += 4;
		DrawText(hdc, str.c_str(), str.size(), &rcText, DT_NOPREFIX);
	}
	else
		DrawText(hdc, str.c_str(), str.size(), &rc, DT_NOPREFIX);
}

void EuhatListCtrl::DrawRunner::drawIconInRect(HDC hdc, HICON icon, RECT &rc, int width, int height)
{
	int rcHeight = rc.bottom - rc.top;
	if (rcHeight > height)
	{
		RECT rcIcon = rc;
		int gap = (rcHeight - height) >> 1;
		rcIcon.top += gap;
		rcIcon.bottom -= gap;
		rcIcon.left += 2;
		EuhatIcon::draw(icon, hdc_, rcIcon.left, rcIcon.top, width, height);
	}
	else
		EuhatIcon::draw(icon, hdc_, rc.left, rc.top, width, height);
}

int EuhatListCtrl::DrawRunner::onColumn(EuhatRect &rc, Column *col, ColumnRow &colRow)
{
	if (colRow.height_ <= 0)
		return 1;

	this_->brushColumnBack_->FillRect(hdc_, rc.rc_);

	RECT rcTxt = rc.rc_;
	if (HAS_ATTR(this_->attr_, EUHAT_LIST_CTRL_ATTR_CHECKBOX) && col == &this_->cols_.cols_.front())
	{
		rcTxt.left = EUHAT_LIST_CTRL_CHECKBOX_WIDTH;
		RECT rcIcon = rc.rc_;
		rcIcon.right = rcTxt.left;
		if (this_->cols_.isSelected_)
			drawIconInRect(hdc_, iconCheckOn_->icon_, rcIcon, EUHAT_LIST_CTRL_CHECKBOX_WIDTH, EUHAT_LIST_CTRL_CHECKBOX_HEIGHT);
		else
			drawIconInRect(hdc_, iconCheckOff_->icon_, rcIcon, EUHAT_LIST_CTRL_CHECKBOX_WIDTH, EUHAT_LIST_CTRL_CHECKBOX_HEIGHT);
	}
	SetTextColor(hdc_, this_->kColumnText);
	drawTextInRect(hdc_, col->name_, rcTxt);

	EuhatGuard<EuhatPen> penGuard(this_->penColumnLine_.get(), hdc_);
	drawLine(hdc_, rc.rc_.left, rc.rc_.bottom - 1, rc.rc_.right, rc.rc_.bottom - 1);
	drawLine(hdc_, rc.rc_.right - 1, rc.rc_.top, rc.rc_.right - 1, rc.rc_.bottom);
	return 1;
}

void EuhatListCtrl::DrawRunner::getDrawText(wstring &txt, Cell *cell, Column *col)
{
	if (NULL != cell)
	{
		if (col->type_ == Column::TypeText)
			txt = cell->value_.get();
		else if (col->type_ == Column::TypeInteger)
		{
			txt = to_wstring(cell->i_);
		}
		else if (col->type_ == Column::TypeDateTime)
		{
			string s = time2Str(cell->i_);
			txt = utf8ToWstr(s.c_str());
		}
	}
}

int EuhatListCtrl::DrawRunner::onCell(EuhatRect &rc, Cell *cell, Column *col, Row &row)
{
	if (row.isSelected_)
	{
		this_->brushCellSelBack_->FillRect(hdc_, rc.rc_);
		SetTextColor(hdc_, this_->kCellSelText);
	}
	else
	{
		this_->brushCellBack_->FillRect(hdc_, rc.rc_);
		SetTextColor(hdc_, this_->kCellText);
	}

	wstring str;
	getDrawText(str, cell, col);

	RECT rcTxt = rc.rc_;
	if (col == &this_->cols_.cols_.front())
	{
		if (HAS_ATTR(this_->attr_, EUHAT_LIST_CTRL_ATTR_CHECKBOX))
		{
			rcTxt.left = EUHAT_LIST_CTRL_CHECKBOX_WIDTH;
			RECT rcIcon = rc.rc_;
			rcIcon.right = rcTxt.left;
			this_->brushCellBack_->FillRect(hdc_, rcIcon);
			if (row.isSelected_)
				drawIconInRect(hdc_, iconCheckOn_->icon_, rcIcon, EUHAT_LIST_CTRL_CHECKBOX_WIDTH, EUHAT_LIST_CTRL_CHECKBOX_HEIGHT);
			else
				drawIconInRect(hdc_, iconCheckOff_->icon_, rcIcon, EUHAT_LIST_CTRL_CHECKBOX_WIDTH, EUHAT_LIST_CTRL_CHECKBOX_HEIGHT);
		}
		if (NULL != row.icon_)
		{
			RECT rcIcon = rcTxt;
			rcTxt.left += EUHAT_LIST_CTRL_ICON_WIDTH;
			rcIcon.right = rcTxt.left;
			this_->brushCellBack_->FillRect(hdc_, rcIcon);
			if (row.isSelected_)
				drawIconInRect(hdc_, row.iconSelected_, rcIcon);
			else
				drawIconInRect(hdc_, row.icon_, rcIcon);
		}
	}
	drawTextInRect(hdc_, str, rcTxt);

	EuhatGuard<EuhatPen> penGuard(this_->penCellLine_.get(), hdc_);
	drawLine(hdc_, rc.rc_.left, rc.rc_.bottom - 1, rc.rc_.right, rc.rc_.bottom - 1);
	drawLine(hdc_, rc.rc_.right - 1, rc.rc_.top, rc.rc_.right - 1, rc.rc_.bottom);
	return 1;
}
