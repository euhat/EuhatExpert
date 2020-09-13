#include "stdafx.h"
#include "HeaderCtrlEx.h"
#include "CtrlExCommon.h"
#include "ListCtrlEx.h"

IMPLEMENT_DYNAMIC(CHeaderCtrlEx, CHeaderCtrl)

CHeaderCtrlEx::CHeaderCtrlEx()
{
	m_Format = "";
	m_Height = 20;
	m_fontHeight = 12;
	m_fontWidth = 0;

	m_iFontOffset = 0;
	m_OffsetColor = RGB(255,255,255);

	COLORREF clr = ctrlExGetListHdrBkgColor();
	m_R = GetRValue(clr);
	m_G = GetGValue(clr);
	m_B = GetBValue(clr);
	m_Gradient = 0;
	m_color = RGB(255,255,255);
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	ON_WM_PAINT()
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
END_MESSAGE_MAP()

void CHeaderCtrlEx::OnPaint()
{
	CPaintDC dc(this);

	if (GetParent()->GetStyle() & LVS_OWNERDRAWFIXED) {
		PaintWithBitmap(&dc, CListCtrlEx::m_hBitmap);
		return;
	}

	int nItem; 
	nItem = GetItemCount();
	for (int i=0; i<nItem; i++) { 
		CRect tRect;
		GetItemRect(i, &tRect);
		int R = m_R, G = m_G, B = m_B;
		CRect nRect(tRect);
		if (i != 0)
			nRect.left++;
		else
			tRect.left += m_iFontOffset;

		for (int j = tRect.top; j <= tRect.bottom; j++) { 
			nRect.bottom = nRect.top + 1; 
			CBrush brush; 
			brush.CreateSolidBrush(RGB(R, G, B));
			dc.FillRect(&nRect, &brush);
			brush.DeleteObject();
			R -= m_Gradient;
			G -= m_Gradient;
			B -= m_Gradient;
			if (R < 0)
				R = 0;
			if (G < 0)
				G = 0;
			if (B < 0)
				B = 0;
			nRect.top = nRect.bottom; 
		} 
		dc.SetBkMode(TRANSPARENT);
		CFont nFont;
		CFont *nOldFont;
		dc.SetTextColor(m_color);
		nFont.CreateFont(m_fontHeight, m_fontWidth, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("ËÎÌו")); 
		nOldFont = dc.SelectObject(&nFont);

		UINT nFormat = 1;
		if (m_Format[i] == '0') {
			nFormat = DT_LEFT;
			tRect.left += 3;
		} else if (m_Format[i] == '1') {
			nFormat = DT_CENTER;
		} else if (m_Format[i] == '2') {
			nFormat = DT_RIGHT;
			tRect.right -= 3;
		}
		TEXTMETRIC metric;
		dc.GetTextMetrics(&metric);
		int ofst = 0;
		ofst = tRect.Height() - metric.tmHeight;
		tRect.OffsetRect(0, ofst / 2);
		CRect rcText = tRect;
		rcText.left += m_iFontOffset;
		dc.DrawText(m_HChar[i], &rcText, nFormat);
		dc.SelectObject(nOldFont); 
		nFont.DeleteObject();
	} 

	CRect rtRect;
	CRect clientRect;
	GetItemRect(nItem - 1, rtRect);
	GetClientRect(clientRect);
	rtRect.left = rtRect.right+1;
	rtRect.right = clientRect.right;
	int R = m_R, G = m_G, B = m_B;
	CRect nRect(rtRect);

	for (int j = rtRect.top; j <= rtRect.bottom; j++) { 
		nRect.bottom = nRect.top + 1; 
		CBrush brush; 
		brush.CreateSolidBrush(RGB(R, G, B));
		dc.FillRect(&nRect, &brush);
		brush.DeleteObject(); 
		R -= m_Gradient;
		G -= m_Gradient;
		B -= m_Gradient;
		if (R < 0)
			R = 0;
		if (G < 0)
			G = 0;
		if (B < 0)
			B = 0;
		nRect.top = nRect.bottom; 
	} 

	if (m_iFontOffset > 0) {
		CPen pen;

		pen.CreatePen(PS_SOLID, m_iFontOffset * 2, m_OffsetColor);
		CPen *pOldPen = dc.SelectObject(&pen);

		int x = 0;
		for (int i = 0; i < nItem; i++) {
			CRect rcItem;
			GetItemRect(i, rcItem);
			x += rcItem.Width();

			dc.MoveTo(x, 0);
			dc.LineTo(x, rcItem.Height());
		}

		dc.SelectObject(pOldPen);
		pen.DeleteObject();
	}
}

LRESULT CHeaderCtrlEx::OnLayout(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam); 
	HD_LAYOUT &hdl = *(HD_LAYOUT *)lParam; 
	RECT *prc = hdl.prc; 
	WINDOWPOS *pwpos = hdl.pwpos; 

	pwpos->cy = m_Height; 
	prc->top = m_Height; 

	GetParent()->Invalidate();

	return lResult;
}

void CHeaderCtrlEx::SetFontOffset(int iOffset, COLORREF OffsetColor)
{
	m_iFontOffset = iOffset;
	m_OffsetColor = OffsetColor;
}

void CHeaderCtrlEx::PaintWithBitmap(CDC *pDC, HBITMAP hBitmap)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	HGDIOBJ pOldBitmap = dcMem.SelectObject(hBitmap);
	BITMAP bmp; 
	GetObject(hBitmap, sizeof(BITMAP), &bmp); 
	pDC->StretchBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);

	int nItem = GetItemCount();
	for (int i = 0; i < nItem; i++) { 
		CRect tRect;
		GetItemRect(i, &tRect);
//		int R = m_R, G = m_G, B = m_B;
		CRect nRect(tRect);
		if (i != 0)
			nRect.left++; // reserve split line
		else
			tRect.left += m_iFontOffset;

		pDC->SetBkMode(TRANSPARENT);
		CFont nFont;
		CFont *nOldFont;
//		dc.SetTextColor(RGB(250, 50, 50)); 
		pDC->SetTextColor(m_color);
		nFont.CreateFont(m_fontHeight, m_fontWidth, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("ËÎÌו"));
		nOldFont = pDC->SelectObject(&nFont);

		UINT nFormat = 1;
		if (m_Format[i] == '0') {
			nFormat = DT_LEFT;
			tRect.left += 3;
		} else if (m_Format[i] == '1') {
			nFormat = DT_CENTER;
		} else if (m_Format[i]=='2') {
			nFormat = DT_RIGHT;
			tRect.right -= 3;
		}
		TEXTMETRIC metric;
		pDC->GetTextMetrics(&metric);
		int ofst = 0;
		ofst = tRect.Height() - metric.tmHeight;
		tRect.OffsetRect(0, ofst / 2);
		CRect rcText = tRect;
		rcText.left += m_iFontOffset;
		pDC->DrawText(m_HChar[i], &rcText, nFormat);
//		dc.DrawText(m_HChar[i], &tRect, nFormat);
		pDC->SelectObject(nOldFont); 
		nFont.DeleteObject();
	} 

	if (m_iFontOffset > 0) {
		CPen pen;
		pen.CreatePen(PS_SOLID, m_iFontOffset * 2, m_OffsetColor);
		CPen *pOldPen = pDC->SelectObject(&pen);

		int x = 0;
		for (int i = 0; i < nItem; i++) {
			CRect rcItem;
			GetItemRect(i, rcItem);
			x += rcItem.Width();
			if (i < nItem - 1) {
				pDC->MoveTo(x, 0);
				pDC->LineTo(x, rcItem.Height());
			} else if (x < rcClient.Width() - m_iFontOffset * 2) {
				pDC->MoveTo(x, 0);
				pDC->LineTo(x, rcItem.Height());
			}
		}
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}
}