#include "stdafx.h"
#include "NewSpinButton.h"
#include "CtrlExCommon.h"

CNewSpinButton::CNewSpinButton()
{
	m_hbmpDownBtn = ctrlExGetConfigBitmap(_T("global"), _T("spinbutton"));
	m_bDown = FALSE;
	m_bFirst = FALSE;

}

CNewSpinButton::~CNewSpinButton()
{
}

BEGIN_MESSAGE_MAP(CNewSpinButton, CSpinButtonCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CNewSpinButton::OnPaint() 
{
	CSpinButtonCtrl::OnPaint();
	CDC *pDC = GetDC();

	CRect rcPic, rcPicup, rcPicdown;
	GetClientRect(&rcPic);

	rcPicup.left = rcPic.left;
	rcPicup.right = rcPic.right;
	rcPicup.top = rcPic.top;
	rcPicup.bottom = rcPic.bottom / 2;

	rcPicdown.left = rcPicup.left;
	rcPicdown.right = rcPicup.right;
	rcPicdown.top = rcPicup.bottom;
	rcPicdown.bottom = rcPic.bottom;

	CBitmap bm;
	bm.Attach(m_hbmpDownBtn);
	CBitmap *pOldBitmap;
	CDC *pMemDC = new CDC;
	pMemDC->CreateCompatibleDC(pDC);
	pOldBitmap = pMemDC->SelectObject(&bm);
	if (m_bDown) {
		if (m_bFirst) {
			pDC->BitBlt(rcPicup.left, rcPicup.top, rcPicup.Width(), rcPicup.Height(), pMemDC, rcPicup.Width(), 0, SRCCOPY);
			pDC->BitBlt(rcPicdown.left, rcPicdown.top, rcPicdown.Width(), rcPicdown.Height(), pMemDC, 0, rcPicdown.Height(), SRCCOPY);
		} else {
			pDC->BitBlt(rcPicup.left, rcPicup.top, rcPic.Width(), rcPicup.Height(), pMemDC, 0, 0, SRCCOPY);
			pDC->BitBlt(rcPicdown.left, rcPicdown.top, rcPicdown.Width(), rcPicdown.Height(), pMemDC, rcPicup.Width(), rcPicdown.Height(), SRCCOPY);
		}			
	} else {
		pDC->BitBlt(rcPicup.left, rcPicup.top, rcPic.Width(), rcPicup.Height(), pMemDC, 0,0, SRCCOPY);
		pDC->BitBlt(rcPicdown.left, rcPicdown.top, rcPicdown.Width(), rcPicdown.Height(), pMemDC, 0, rcPicdown.Height(),SRCCOPY);
	}

	pMemDC->SelectObject(pOldBitmap);
	delete pMemDC;
	ReleaseDC(pDC);
	bm.Detach();
	// Do not call CSpinButtonCtrl::OnPaint() for painting messages
}

void CNewSpinButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rc;
	GetClientRect(&rc);
	if (point.y < rc.bottom / 2) {
		m_bFirst = TRUE;
	} else {
		m_bFirst = FALSE;
	}
	m_bDown = TRUE;
	CSpinButtonCtrl::OnLButtonDown(nFlags, point);
}

void CNewSpinButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDown = FALSE;
	CSpinButtonCtrl::OnLButtonUp(nFlags, point);
}
