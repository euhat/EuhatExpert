#include "stdafx.h"
#include "JyListBox.h"
#include <EuhatChildWnd.h>
#include <EuhatPostDefMfc.h>

BEGIN_MESSAGE_MAP(JyListBox, CListBox)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

JyListBox::JyListBox()
{
	font_.reset(new EuhatFont(_T("MS Shell Dlg"), 18));
}

void JyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int count = GetCount();
	if (count <= 0)
		return;

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	JyListBox::Item *item = (JyListBox::Item *)GetItemDataPtr(lpDrawItemStruct->itemID);

	CString str;
	//GetText(lpDrawItemStruct->itemID, str);
	str = item->str_;
	if (lpDrawItemStruct->itemAction | ODA_SELECT && lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		CBrush brush;
		brush.CreateSolidBrush(RGB(200, 200, 200));
		dc.FillRect(&lpDrawItemStruct->rcItem, &brush);
	}
	else
	{
		CBrush brush;
		brush.CreateSolidBrush(RGB(255, 255, 255));
		dc.FillRect(&lpDrawItemStruct->rcItem, &brush);
	}

	dc.SetBkMode(TRANSPARENT);
	if (lpDrawItemStruct->itemState & ODS_DISABLED)
		dc.SetTextColor(RGB(220, 220, 220));
	else
		dc.SetTextColor(RGB(46, 134, 208));

	{
		EuhatGuard<EuhatFont> fontGuard(font_.get(), dc.m_hDC);
		dc.TextOut(8, lpDrawItemStruct->rcItem.top + 8, str, str.GetLength());
	}

	dc.Detach();
}

void JyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 43;
}

void JyListBox::insertStr(int iIndex, CString str, void *userData)
{
	JyListBox::Item *item = new JyListBox::Item();
	item->str_ = str;
	item->userData_ = userData;

	int i = SetItemDataPtr(InsertString(iIndex, _T("")), item);
}

JyListBox::Item *JyListBox::getItem(int idx)
{
	if (idx < 0 || idx >= GetCount())
		return NULL;

	return (JyListBox::Item *)GetItemDataPtr(idx);
}

void JyListBox::ResetContent()
{
	int iCount = GetCount();
	for (int i = 0; i < iCount; i++)
	{
		JyListBox::Item *item = (JyListBox::Item *)GetItemDataPtr(i);
		delete item;
	}
	CListBox::ResetContent();
}

void JyListBox::OnDestroy()
{
	ResetContent();

	CListBox::OnDestroy();
}

BOOL JyListBox::OnEraseBkgnd(CDC *pDC)
{
	
	return CListBox::OnEraseBkgnd(pDC);
}

void JyListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	
	CListBox::OnMouseMove(nFlags, point);
}