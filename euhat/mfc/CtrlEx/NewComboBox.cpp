#include "stdafx.h"
#include "NewComboBox.h"
#include "CtrlExCommon.h"

CNewComboBox::CNewComboBox()
{
	m_bDown = FALSE; 
	m_hbmpDownBtn = ctrlExGetConfigBitmap(_T("global"), _T("combobox"));

}

CNewComboBox::~CNewComboBox()
{
}

BEGIN_MESSAGE_MAP(CNewComboBox, CComboBox)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CNewComboBox::OnPaint() 
{
	CComboBox::OnPaint();
	CDC *dc = GetDC();
//	CPaintDC dc(this);
	
	CRect rcPic;
	GetClientRect(&rcPic);
	rcPic.left = rcPic.right - ctrlExGetBitmapWidth(m_hbmpDownBtn) / 4;
	rcPic.top += 1;
	rcPic.bottom -= 1;
	CBitmap bm;
	bm.Attach(m_hbmpDownBtn);

	CBitmap *pOldBitmap;
	CDC *pMemDC = new CDC;
	CDC *pDC = dc;
	pMemDC->CreateCompatibleDC(pDC);
	pOldBitmap = pMemDC->SelectObject(&bm);
	if (this->IsWindowEnabled()) {
		if (m_bDown) {
			pDC->BitBlt(rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), pMemDC, rcPic.Width() * 2, 0, SRCCOPY);
		} else {		
			pDC->BitBlt(rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), pMemDC, 0, 0, SRCCOPY);
		}
	} else {
		pDC->BitBlt(rcPic.left, rcPic.top, rcPic.Width(), rcPic.Height(), pMemDC, rcPic.Width() * 3, 0, SRCCOPY);
	}

	pMemDC->SelectObject(pOldBitmap);

	ReleaseDC(pMemDC);
	delete pMemDC;
	ReleaseDC(dc);
	bm.Detach();
	// Do not call CComboBox::OnPaint() for painting messages
}

void CNewComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDown = TRUE;	
	Invalidate();
	CComboBox::OnLButtonDown(nFlags, point);
}

void CNewComboBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDown = FALSE;
	Invalidate();
	CComboBox::OnLButtonUp(nFlags, point);
}
