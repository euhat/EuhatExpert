#include "stdafx.h"
#include "DateTime.h"
#include "CtrlExCommon.h"

CNewDateCtrl::CNewDateCtrl()
{
	m_hbmpDownBtn = ctrlExGetConfigBitmap(_T("global"), _T("combobox"));
	m_bDown = FALSE;
}

CNewDateCtrl::~CNewDateCtrl()
{
}

BEGIN_MESSAGE_MAP(CNewDateCtrl, CDateTimeCtrl)
	ON_WM_PAINT()
	ON_WM_MBUTTONUP()
	ON_WM_NCLBUTTONUP()
	ON_NOTIFY_REFLECT(DTN_DROPDOWN, OnDropdown)
	ON_NOTIFY_REFLECT(NM_SETFOCUS, OnSetfocus)
	ON_NOTIFY_REFLECT(DTN_CLOSEUP, OnCloseup)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CNewDateCtrl::OnPaint() 
{
//	CPaintDC dc(this);
	CDateTimeCtrl::OnPaint();
	CDC* dc = GetDC();
	
	CRect rcPic;
	GetClientRect(&rcPic);
	rcPic.left = rcPic.right - ctrlExGetBitmapWidth(m_hbmpDownBtn)/4;
//	rcPic.top += 1;
//	rcPic.bottom -= 1;

	CBitmap bm;
	bm.Attach(m_hbmpDownBtn);
	CBitmap *pOldBitmap;
	CDC *pMemDC = new CDC;
	CDC *pDC = dc;

	pMemDC->CreateCompatibleDC(pDC);
	pOldBitmap = pMemDC->SelectObject(&bm);

	if (m_bDown  || (GetMonthCalCtrl() != NULL)) {	
		pDC->BitBlt(rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), pMemDC, rcPic.Width(), 0, SRCCOPY);
	} else {
		pDC->BitBlt(rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), pMemDC, 0, 0, SRCCOPY);
	}
	
	pMemDC->SelectObject(pOldBitmap);

	ReleaseDC(pMemDC);
	delete pMemDC;
	ReleaseDC(dc);
	bm.Detach();


	// Do not call CDateTimeCtrl::OnPaint() for painting messages
}

void CNewDateCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDown = TRUE;	
	Invalidate();
	SetTimer(1,200,NULL);

/*	if (GetMonthCalCtrl() == NULL) {
	} else {
		KillTimer(1);
	}
*/	
	CDateTimeCtrl::OnLButtonDown(nFlags, point);
}

void CNewDateCtrl::OnTimer(UINT_PTR nIDEvent)
{
	Invalidate();
	CDateTimeCtrl::OnTimer(nIDEvent);
}

void CNewDateCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CDateTimeCtrl::OnKillFocus(pNewWnd);
	KillTimer(1);
}

void CNewDateCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDown = FALSE;	
	Invalidate();
//	KillTimer(1);
	CDateTimeCtrl::OnLButtonUp(nFlags, point);
}

void CNewDateCtrl::OnDropdown(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	m_bDown = TRUE;		
//	COLORREF clr = RGB(100,100,100);
//	SetMonthCalColor(MCSC_MONTHBK,clr);
	Invalidate();
	
	*pResult = 0;
}

void CNewDateCtrl::OnSetfocus(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	m_bDown = FALSE;	
	Invalidate();

	*pResult = 0;
}

void CNewDateCtrl::OnCloseup(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	m_bDown = FALSE;	
	Invalidate();

	*pResult = 0;
}

