#include "stdafx.h"
#include "ListCtrlEx.h"
#include "CtrlExCommon.h"

struct stColor
{
	int nRow;
	int nCol;
	COLORREF rgb;
};

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

HBITMAP CListCtrlEx::m_hBitmap = NULL;

CListCtrlEx::CListCtrlEx()
	: m_nRowHeight(20), m_fontHeight(12), m_fontWidth(0)
{
	m_clrText = RGB(0,0,0);
	m_clrGrid = RGB(255,255,255);
	m_clrHighlight  = GetSysColor(COLOR_HIGHLIGHT) ;
   
	if (m_hBitmap == NULL) {
		m_hBitmap = ctrlExGetConfigBitmap(_T("global"), _T("listctrlbkg"));
	}
}

CListCtrlEx::~CListCtrlEx()
{
	DeletePtrList(m_ptrListCol);
	DeletePtrList(m_ptrListItem);
	DeletePtrList(m_colTextColor);
	DeletePtrList(m_ItemTextColor);
	m_ptrListCol.RemoveAll();
	m_ptrListItem.RemoveAll();
	m_colTextColor.RemoveAll();
	m_ItemTextColor.RemoveAll();

// 	if (m_hBitmap != NULL) {
// 		DeleteObject(m_hBitmap);
// 		m_hBitmap = NULL;
// 	}
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CListCtrlEx::DeletePtrList(CPtrList &ptrList)
{
	for (POSITION pos = ptrList.GetHeadPosition(); pos != NULL;) {
		stColor *pColor = (stColor *)ptrList.GetNext(pos);
		if (pColor != NULL) {
			delete pColor;
			pColor = NULL;
		}
	}
}

void CListCtrlEx::PreSubclassWindow()
{
//	ModifyStyle(0, LVS_OWNERDRAWFIXED);
	CListCtrl::PreSubclassWindow();
	CHeaderCtrl *pHeader = GetHeaderCtrl();
	m_Header.SubclassWindow(pHeader->GetSafeHwnd());
}

void CListCtrlEx::OnPaint()
{
//	CPaintDC dc(this);
	CListCtrl::OnPaint();
	
	if ((GetStyle() & LVS_OWNERDRAWFIXED) == 0) {
		return;
	}

	CDC *pDC = GetDC();
	CRect rcClient, rcHeader;
	GetClientRect(&rcClient);
	m_Header.GetClientRect(&rcHeader);
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	HGDIOBJ pOldBitmap = dcMem.SelectObject(m_hBitmap);
	BITMAP bmp; 
	GetObject(m_hBitmap, sizeof(BITMAP), &bmp); 

	TCHAR lpBuffer[256];
	LV_ITEM lvi;
 	int nTop = GetTopIndex();
 	int nLast = nTop + GetCountPerPage();
	int i = 0;
	for (i = nTop; i <= nLast; i++) {
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = i;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iSubItem = 0;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		if (!GetItem(&lvi)) {
			if ((i - nTop) * m_nRowHeight < rcClient.bottom) {
				pDC->StretchBlt(0, rcHeader.bottom + m_nRowHeight * (i - nTop), rcClient.Width(), m_nRowHeight, &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			}
		}
	}
	dcMem.SelectObject(pOldBitmap);

//	CDC *pDC = GetDC();
	CPen penRow, penCol;
	penRow.CreatePen(PS_SOLID, GRID_ROW_PIXEL, m_clrGrid);
	penCol.CreatePen(PS_SOLID, GRID_COL_PIXEL, m_clrGrid);
	CPen *pOldPen = pDC->SelectObject(&penRow);
	
	GetClientRect(&rcClient);
	m_Header.GetClientRect(&rcHeader);
	
	int x = 0;
	int y = rcHeader.Height() + 1;
	int cx = rcClient.Width();
	int cy = m_nRowHeight;
	
	// row line
//	int nTop = GetTopIndex();
//	int nLast = nTop + GetCountPerPage();	
	for (i = nTop; i <= nLast; i++) {
		pDC->MoveTo(x, y + (i - nTop) * cy);
		pDC->LineTo(cx, y + (i - nTop) * cy);
	}
	
	// column line
    pDC->SelectObject(&penCol);
	m_Header.GetWindowRect(&rcHeader);
	ScreenToClient(&rcHeader);
	x = rcHeader.left;
	int nCol = m_Header.GetItemCount();
   
    for (i = 0; i < nCol; i++) {
		m_Header.GetItemRect(i, rcHeader);
		x += rcHeader.Width();
		
		if (i < nCol - 1) {
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rcClient.Height());
		} else if (x < cx - GRID_COL_PIXEL) {
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rcClient.Height());
		}
	}
	
	pDC->SelectObject(pOldPen);
	
	ReleaseDC(pDC);

	penRow.DeleteObject();
	penCol.DeleteObject();
}

void CListCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC *pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    // draw blank line
	CRect rcClient, rcItem;
	GetClientRect(&rcClient);
	GetItemRect(lpDrawItemStruct->itemID, &rcItem, LVIR_BOUNDS);
	if (rcItem.bottom > rcClient.bottom) {
		return;
	}
    CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	HGDIOBJ pOldBitmap = dcMem.SelectObject(m_hBitmap);
	BITMAP bmp; 
	GetObject(m_hBitmap, sizeof(BITMAP), &bmp); 
	pDC->StretchBlt(0, rcItem.top, rcClient.Width(), rcItem.Height(), &dcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);

	TCHAR lpBuffer[256];
    int nCol = m_Header.GetItemCount();	
	LV_ITEM lvi;
	for (int i = 0; i < nCol; i++) {
		CRect rcText;
		CRect rcIcon;
		if (!GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_LABEL, rcText))
			continue;

		// Draw the button frame.   
//		::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);  

		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		lvi.iSubItem = i;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
		
		if (lvi.iImage > 0) {
			CImageList *pList = GetImageList(LVSIL_SMALL);
			if (pList != NULL) {
				if (GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_ICON, rcIcon)) {
					pList->Draw(pDC, lvi.iImage, CPoint(rcIcon.left, rcIcon.top), ILD_TRANSPARENT);
					rcText.left = rcIcon.right;
				}
			}
		}
		CRect rcTemp;
		rcTemp = rcText;
   
		if (lpDrawItemStruct->itemState & ODS_SELECTED) {
			pDC->FillSolidRect(&rcTemp, m_clrHighlight /* GetSysColor(COLOR_HIGHLIGHT) */);
		//	pDC->SetTextColor(m_clrHighlight /* GetSysColor(COLOR_HIGHLIGHTTEXT) */);
		} else {
// 			COLORREF color;
// 			if (lpDrawItemStruct->itemID % 2 == 0) {
//              	color = GetBkColor();
// 			} else {
//                	color = RGB(55, 55, 55);
// 			}
	
//			pDC->FillSolidRect(rcTemp,color);
// 
// 			if (FindColColor(i, color)) {
// 				pDC->FillSolidRect(rcTemp, color);
// 			}
// 			if (FindItemColor(i, lpDrawItemStruct->itemID, color)) {
// 				pDC->FillSolidRect(rcTemp,color);
// 			}
			
//			pDC->SetTextColor(m_color);
		}

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

		UINT uFormat = DT_CENTER;
		if (m_Header.m_Format[i] == '0') {
			uFormat = DT_LEFT;
		} else if (m_Header.m_Format[i] == '1') {
			uFormat = DT_CENTER;
		} else if (m_Header.m_Format[i] == '2') {
			uFormat = DT_RIGHT;
		}
		TEXTMETRIC metric;
		pDC->GetTextMetrics(&metric);
		int ofst;
		ofst = rcTemp.Height() - metric.tmHeight;
		rcTemp.OffsetRect(0, ofst / 2);

		if (lpDrawItemStruct->itemState & ODS_SELECTED) {
			pDC->SetTextColor(RGB(0, 0, 0));
		} else {
			pDC->SetTextColor(m_clrText);
		}
		
		COLORREF color;
		if (FindColTextColor(i,color)) {
			pDC->SetTextColor(color);
		}
		if (FindItemTextColor(i, lpDrawItemStruct->itemID, color)) {
			pDC->SetTextColor(color);
		}
		CFont *nOldFont;
		nOldFont = pDC->SelectObject(&font_);
		rcTemp.left += 2 + GRID_COL_PIXEL / 2;
		DrawText(lpDrawItemStruct->hDC, lpBuffer, _tcsclen(lpBuffer), &rcTemp, uFormat);
		pDC->SelectStockObject(SYSTEM_FONT);
	}
}

void CListCtrlEx::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CListCtrlEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight > 0) {
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	} else {
		m_nRowHeight = lpMeasureItemStruct->itemHeight;
	}
}

int CListCtrlEx::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat /* = LVCFMT_LEFT */, int nWidth /* = -1 */, int nSubItem /* = -1 */)
{
	m_Header.m_HChar.Add(lpszColumnHeading);
	if (nFormat == LVCFMT_LEFT) {
		m_Header.m_Format = m_Header.m_Format + _T("0");
	} else if (nFormat == LVCFMT_CENTER) {
		m_Header.m_Format = m_Header.m_Format + _T("1");
	} else if (nFormat == LVCFMT_RIGHT) {
		m_Header.m_Format = m_Header.m_Format + _T("2");
	} else {
		m_Header.m_Format = m_Header.m_Format + _T("1");
	}
	return CListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
}

int CListCtrlEx::InsertColumn(int nCol, const LVCOLUMN *pColumn)
{
  return InsertColumn(nCol, pColumn->pszText, pColumn->fmt, pColumn->cx, pColumn->iSubItem);
}

BOOL CListCtrlEx::DeleteColumn(int nCol)
{
	if (m_Header.m_HChar.GetCount() > nCol) {
		m_Header.m_HChar.RemoveAt(nCol);
		return CListCtrl::DeleteColumn(nCol);
	}
	return FALSE;
}

void CListCtrlEx::SetHeaderBKColor(int r, int g, int b, int gradient)
{
	m_Header.m_R = r;
	m_Header.m_G = g;
	m_Header.m_B = b;
	m_Header.m_Gradient = gradient;
}

void CListCtrlEx::SetHeaderHeight(int Height)
{
	m_Header.m_Height = Height;
}

bool CListCtrlEx::FindColColor(int col, COLORREF &color)
{
	int flag = 0;
	for (POSITION pos = m_ptrListCol.GetHeadPosition(); pos != NULL;) {
		stColor *pColor = (stColor *)m_ptrListCol.GetNext(pos);
		if (pColor->nCol == col) {
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1 == flag) {
		return true;
	}
	return false;
}

bool CListCtrlEx::FindItemColor(int col,int row,COLORREF &color)
{
	int flag = 0;
	for (POSITION pos = m_ptrListItem.GetHeadPosition(); pos != NULL;) {
		stColor *pColor = (stColor *)m_ptrListItem.GetNext(pos);
		if (pColor->nCol == col && pColor->nRow == row) {
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1 == flag) {
		return true;
	}
	return false;
}

void CListCtrlEx::SetColColor(int col, COLORREF color)
{
	stColor *pColor  = new stColor;
	pColor->nCol = col;
	pColor->rgb = color;
	m_ptrListCol.AddTail(pColor);
}

void CListCtrlEx::SetItemColor(int col, int row, COLORREF color)
{
	stColor *pColor  = new stColor;
	pColor->nCol = col;
	pColor->nRow = row;
	pColor->rgb = color;
	m_ptrListItem.AddTail(pColor);
}

void CListCtrlEx::SetRowHeigt(int nHeight)
{
	m_nRowHeight = nHeight;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

void CListCtrlEx::SetHeaderFontHW(int nHeight, int nWidth)
{
	m_Header.m_fontHeight = nHeight;
	m_Header.m_fontWidth = nWidth;
}

void CListCtrlEx::SetHeaderTextColor(COLORREF color)
{
	m_Header.m_color = color;
}

BOOL CListCtrlEx::SetTextColor(COLORREF cr)
{
	m_clrText = cr;
	CListCtrl::SetTextColor(cr);
	return TRUE;
}

void CListCtrlEx::SetFontHW(int nHeight, int nWidth)
{
	m_fontHeight = nHeight;
	m_fontWidth = nWidth;
	font_.DeleteObject();
	font_.CreateFont(m_fontHeight, m_fontWidth, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _TEXT("ËÎÌו"));
}

void CListCtrlEx::SetColTextColor(int col, COLORREF color)
{
	stColor *pColor = new stColor;
	pColor->nCol = col;
	pColor->rgb = color;
	m_colTextColor.AddTail(pColor);
}

bool CListCtrlEx::FindColTextColor(int col, COLORREF &color)
{
	int flag = 0;
	for (POSITION pos = m_colTextColor.GetHeadPosition(); pos != NULL;) {
		stColor *pColor = (stColor *)m_colTextColor.GetNext(pos);
		if (pColor->nCol == col) {
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1 == flag) {
		return true;
	}
	return false;
}

bool CListCtrlEx::FindItemTextColor(int col, int row, COLORREF &color)
{
	int flag = 0;
	for (POSITION pos = m_ItemTextColor.GetHeadPosition(); pos != NULL;) {
		stColor *pColor = (stColor *)m_ItemTextColor.GetNext(pos);
		if (pColor->nCol == col && pColor->nRow == row) {
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1 == flag) {
		return true;
	}
	return false;
}

void CListCtrlEx::SetItemTextColor(int col, int row, COLORREF color)
{
	stColor *pColor = new stColor;
	pColor->nCol = col;
	pColor->nRow = row;
	pColor->rgb = color;
	m_ItemTextColor.AddTail(pColor);
}

void CListCtrlEx::SetOwnerDrawStyle()
{   
	ModifyStyle(0, LVS_OWNERDRAWFIXED);
 
	m_clrHighlight = RGB(145, 145, 145);
    m_clrGrid = RGB(0, 0, 0);
	SetTextBkColor(RGB(45, 45, 45));
	SetBkColor(RGB(45, 45, 45));
	SetTextColor(RGB(255, 255, 255));
	SetHeaderBKColor(58, 60, 63);

	SetHeaderHeight(m_nRowHeight);
    m_Header.SetFontOffset(GRID_COL_PIXEL / 2, m_clrGrid);
}

void CListCtrlEx::OnDestroy()
{
	if (m_Header.GetSafeHwnd() != NULL) {
		m_Header.UnsubclassWindow();
	}
	
	CListCtrl::OnDestroy();
}