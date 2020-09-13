#include <EuhatPreDef.h>
#include <Windows.h>
#include "EuhatListCtrl.h"
#include <EuhatPostDef.h>

using namespace euhatwin;

EuhatListCtrl::EventRunner::EventRunner(EuhatListCtrl *pThis)
	: selectRc_(0, 0, 0, 0)
{
	this_ = pThis;

	col_ = NULL;
	row_ = NULL;
	cell_ = NULL;

	needRefresh_ = 0;
	isMouseDown_ = 0;

	isInSelectCheckBoxMode_ = 0;
	isInSelectColumnMode_ = 0;
}

void EuhatListCtrl::EventRunner::onEvent(WPARAM wParam, int x, int y, MouseEvent mouseEvent)
{
	POINT pt; pt.x = x; pt.y = y;

	mouseEvent_ = mouseEvent;
	isInSelectColumnMode_ = !isMouseDown_ && y < this_->cols_.height_;
	isInSelectCheckBoxMode_ = !isMouseDown_ && (this_->attr_ & EUHAT_LIST_CTRL_ATTR_CHECKBOX) && x <= EUHAT_LIST_CTRL_CHECKBOX_WIDTH;

	if (isInSelectColumnMode_)
	{
		selectRc_.rc_.left = pt.x;
		selectRc_.rc_.top = pt.y;
		runColumns();
		needRefresh_ = 1;
	}
	else
	{
		this_->ui2logical(pt);
		if (isInSelectCheckBoxMode_)
		{
			selectRc_.rc_.left = pt.x;
			selectRc_.rc_.top = pt.y;
		}
		else
		{
			if (mouseEvent_ == MouseLBtnDown)
			{
				::SetCapture(this_->hwnd_);
				if (!(wParam & MK_CONTROL))
					setAllSelected(0);
				if (!(wParam & MK_SHIFT))
				{
					selectRc_.rc_.left = pt.x;
					selectRc_.rc_.top = pt.y;
				}
				selectRc_.rc_.right = pt.x;
				selectRc_.rc_.bottom = pt.y;
				isMouseDown_ = 1;
			}
			else if (mouseEvent_ == MouseLBtnUp)
			{
				::ReleaseCapture();
				selectRc_.rc_.right = pt.x;
				selectRc_.rc_.bottom = pt.y;
				isMouseDown_ = 0;
				needRefresh_ = 1;
			}
			else
			{
				if (isMouseDown_)
				{
					if (!(wParam & MK_CONTROL))
						setAllSelected(0);
					selectRc_.rc_.right = pt.x;
					selectRc_.rc_.bottom = pt.y;
					needRefresh_ = 1;
				}
				else
				{
					selectRc_.rc_.left = pt.x;
					selectRc_.rc_.top = pt.y;
				}
			}
		}
		curPos_.x = curPos_.y = 0;
		GetClientRect(this_->hwnd_, &winRc_.rc_);
		this_->ui2logical(winRc_);

		runRows();
	}

	if (needRefresh_)
	{
		needRefresh_ = 0;
		InvalidateRect(this_->hwnd_, NULL, 0);
	}
}

int EuhatListCtrl::EventRunner::onColumn(EuhatRect &rc, Column *col, ColumnRow &colRow)
{
	if (rc.isIn(selectRc_.rc_.left, selectRc_.rc_.top))
	{
		if (mouseEvent_ == MouseLBtnDown)
		{
			if (selectRc_.rc_.left <= EUHAT_LIST_CTRL_CHECKBOX_WIDTH)
			{
				this_->cols_.isSelected_ = !this_->cols_.isSelected_;
				setAllSelected(this_->cols_.isSelected_);
			}
			else
			{
				int i = 0;
				for (list<Column>::iterator it = colRow.cols_.begin(); it != colRow.cols_.end(); it++, i++)
				{
					if (col == &*it)
						break;
				}
				this_->rows_.sort([this, col, i](Row &l, Row &r){
					int j = 0;
					list<Cell>::iterator itL;
					for (itL = l.cells_.begin(); itL != l.cells_.end() && j < i; itL++, j++)
						;
					j = 0;
					list<Cell>::iterator itR;
					for (itR = r.cells_.begin(); itR != r.cells_.end() && j < i; itR++, j++)
						;
					int64_t comp;
					if (col->type_ == Column::TypeText)
					{
						comp = opStrCmpNoCase(itL == l.cells_.end()? L"" : itL->value_.get(), itR == r.cells_.end()? L"" : itR->value_.get());
					}
					else
						comp = (itL == l.cells_.end()? 0 : itL->i_) - (itR == r.cells_.end()? 0 : itR->i_);
					if (!col->isAscending_)
						comp = -comp;
					return comp < 0;
				});
				for (list<Column>::iterator it = colRow.cols_.begin(); it != colRow.cols_.end(); it++, i++)
				{
					if (col != &*it)
						it->isAscending_ = 0;
				}
				col->isAscending_ = !col->isAscending_;
			}
		}
		return 0;
	}
	return 1;
}

int EuhatListCtrl::EventRunner::onCell(EuhatRect &rc, Cell *cell, Column *col, Row &row)
{
	if (isInSelectCheckBoxMode_)
	{
		if (mouseEvent_ != MouseLBtnDown || !rc.isIn(selectRc_.rc_.left, selectRc_.rc_.top))
			return 1;

		row.isSelected_ = !row.isSelected_;
		this_->cols_.isSelected_ = 0;
		needRefresh_ = 1;
		return 1;
	}
	else
	{
		if (!isMouseDown_ && rc.isIn(selectRc_.rc_.left, selectRc_.rc_.top) && cell_ != cell)
		{
			wstring txt;
			this_->drawRunner_->getDrawText(txt, cell, col);
			toolTip_.reset(new EuhatToolTip(this_->hwnd_, txt.c_str()));
			EuhatRect rcTt;
			rcTt.rc_ = rc.rc_;
			this_->logical2ui(rcTt);
			EuhatRect rcWin;
			GetWindowRect(this_->hwnd_, &rcWin.rc_);
			rcTt.move(rcWin.rc_.left, rcWin.rc_.top);
			if (col == &this_->cols_.cols_.front())
			{
				if (HAS_ATTR(this_->attr_, EUHAT_LIST_CTRL_ATTR_CHECKBOX))
					rcTt.rc_.left += EUHAT_LIST_CTRL_CHECKBOX_WIDTH;
				if (NULL != row.icon_)
					rcTt.rc_.left += EUHAT_LIST_CTRL_ICON_WIDTH;
			}
			rcTt.rc_.left += 4;
			rcTt.rc_.top--;
			toolTip_->moveWindow(&rcTt.rc_);
			::UpdateWindow(toolTip_->hwnd_);

			EuhatRect rcTrue;
			GetClientRect(toolTip_->hwnd_, &rcTrue.rc_);
			if (col == &this_->cols_.cols_.front())
			{
				if (HAS_ATTR(this_->attr_, EUHAT_LIST_CTRL_ATTR_CHECKBOX))
					rcTrue.rc_.left -= EUHAT_LIST_CTRL_CHECKBOX_WIDTH;
				if (NULL != row.icon_)
					rcTrue.rc_.left -= EUHAT_LIST_CTRL_ICON_WIDTH;
			}
			if (rcTrue.width() < rc.width())
				toolTip_.reset();
			cell_ = cell;
		}
		if (!isMouseDown_ || !selectRc_.isIntersect(rc.rc_))
			return 1;

		if (this_->attr_ & EUHAT_LIST_CTRL_ATTR_MULTILINE)
		{
			if (this_->attr_ & EUHAT_LIST_CTRL_ATTR_WHOLE_LINE)
			{
				row.isSelected_ = 1;
			}
		}
	}

	return 1;
}

void EuhatListCtrl::EventRunner::setAllSelected(int selected)
{
	for (list<Row>::iterator it = this_->rows_.begin(); it != this_->rows_.end(); it++)
	{
		it->isSelected_ = selected;
	}
	this_->cols_.isSelected_ = selected;
}