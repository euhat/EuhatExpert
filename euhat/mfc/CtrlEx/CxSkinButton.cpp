#include "stdafx.h"
#include "CxSkinButton.h"
#include "CtrlExCommon.h"

BOOL ExtractBitmap(CBitmap *pSource, CBitmap *pDest, CRect rcExtract)
{
	HWND hWnd = ::GetDesktopWindow();
	if (!pSource || !pSource->m_hObject || !hWnd || !pDest) {
		return FALSE;
	}

	HDC hDC = ::GetDC(hWnd);
	CDC dc;
	dc.Attach(hDC);

	CDC memDC, memDC2;
	memDC.CreateCompatibleDC(&dc);
	memDC2.CreateCompatibleDC(&dc);
	pDest->DeleteObject();
	pDest->CreateCompatibleBitmap(&dc, rcExtract.Width(), rcExtract.Height());
	CBitmap *pOld = memDC.SelectObject(pDest);
	CBitmap *pOld2 = memDC2.SelectObject(pSource);

	memDC.BitBlt(0, 0, rcExtract.Width(), rcExtract.Height(), &memDC2,
		rcExtract.left, rcExtract.top, SRCCOPY);

	memDC.SelectObject(pOld);
	memDC2.SelectObject(pOld2);

	dc.Detach();
	::ReleaseDC(hWnd, hDC);

	return TRUE;
}

CxSkinButton::CxSkinButton()
{
	m_DrawMode = 1;			// normal drawing mode
	m_FocusRectMargin = 0;	// disable focus dotted rect
	m_hClipRgn = NULL;		// no clipping region
	m_TextColor = GetSysColor(COLOR_BTNTEXT);	// default button text color
	m_button_down = m_tracking = false;
	m_hCursor = NULL;
	m_nSetBtnOrder = 0;
	m_bMouseDown = FALSE;

	m_OffState = FALSE;
}

CxSkinButton::~CxSkinButton()
{
	if (m_hClipRgn) {
		DeleteObject(m_hClipRgn);
	}

	m_bmpNormal.DeleteObject();
	m_bmpDown.DeleteObject();
	m_bmpOver.DeleteObject();
	m_bmpDisabled.DeleteObject();
	m_bmpMask.DeleteObject();
	m_bmpOff.DeleteObject();
	m_bmpOff.DeleteObject();
}

BEGIN_MESSAGE_MAP(CxSkinButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CxSkinButton::OnEraseBkgnd(CDC* pDC) 
{
	return 0;
}

void CxSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	POINT mouse_position;
	if (m_button_down && ::GetCapture() == m_hWnd && ::GetCursorPos(&mouse_position)) {
		if (::WindowFromPoint(mouse_position) == m_hWnd) {
			if ((GetState() & BST_PUSHED) != BST_PUSHED) {
				SetState(TRUE);
				return;
			}
		} else {
			if ((GetState() & BST_PUSHED) == BST_PUSHED) {
				SetState(FALSE);
				return;
			}
		}
	}

	CString sCaption;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	// get device context
	RECT r = lpDrawItemStruct->rcItem;					// context rectangle
	int cx = r.right - r.left;							// get width
	int cy = r.bottom - r.top;							// get height

	GetWindowText(sCaption);							// get button text
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT)); //get text font

#if USE_TEXT_METRIC
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);							// get font metrics
	// get top-left corner to draw the text centered on the button
	int tx = (cx - tm.tmAveCharWidth * sCaption.GetLength()) / 2;
	int ty = (cy - tm.tmHeight) / 2;
#else
	CSize cs = pDC->GetTextExtent(sCaption);
	int tx = (cx - cs.cx) / 2;
	int ty = (cy - cs.cy) / 2;
#endif

	// Select the correct skin 
	if (lpDrawItemStruct->itemState & ODS_DISABLED){	// DISABLED BUTTON
		if (m_bmpDisabled.m_hObject == NULL) {
			// no skin selected for disabled state -> standard button
			pDC->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));
		} else {
			// paint the skin
			DrawBitmap(pDC, (HBITMAP)m_bmpDisabled, r, m_DrawMode);
		}
		// if needed, draw the standard 3D rectangular border
		if (m_bBorder) pDC->DrawEdge(&r, EDGE_RAISED, BF_RECT);
		// paint the etched button text
		pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
		pDC->ExtTextOut(tx + 1, ty + 1, ETO_CLIPPED, &r, sCaption, NULL);
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		pDC->ExtTextOut(tx, ty, ETO_CLIPPED, &r, sCaption, NULL);
	} else {										// SELECTED (DOWN) BUTTON
		if (lpDrawItemStruct->itemState & ODS_SELECTED){
			if (m_bmpDown.m_hObject == NULL) {
				// no skin selected for selected state -> standard button
				pDC->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));
			} else {
				// paint the skin
				if (m_OffState && (m_bmpOff.m_hObject != NULL)) {
					DrawBitmap(pDC, (HBITMAP)m_bmpOff, r, m_DrawMode);
				} else {
					DrawBitmap(pDC, (HBITMAP)m_bmpDown, r, m_DrawMode);
				}
			}
			// if needed, draw the standard 3D rectangular border
			if (m_bBorder) pDC->DrawEdge(&r, EDGE_SUNKEN, BF_RECT);
		} else {									// DEFAULT BUTTON
			if (m_bmpNormal.m_hObject == NULL) {
				// no skin selected for normal state -> standard button
				pDC->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));
			} else {
				if ((m_tracking || m_bMouseDown) && (m_bmpOver.m_hObject != NULL)) {
					if (m_OffState && (m_bmpOffOn.m_hObject != NULL)) {
						DrawBitmap(pDC, (HBITMAP)m_bmpOffOn, r, m_DrawMode);
					}
					if (m_OffState && (m_bmpOffOn.m_hObject == NULL) && (m_bmpOff.m_hObject != NULL)) {
						DrawBitmap(pDC, (HBITMAP)m_bmpOff, r, m_DrawMode);
					} else if (m_OffState == FALSE) {
						DrawBitmap(pDC, (HBITMAP)m_bmpOver, r, m_DrawMode);
					}
				} else {
					if (m_OffState && (m_bmpOff.m_hObject != NULL)) {
						DrawBitmap(pDC, (HBITMAP)m_bmpOff, r, m_DrawMode);
					} else {
						DrawBitmap(pDC, (HBITMAP)m_bmpNormal, r, m_DrawMode);
					}
				}
			}
			// if needed, draw the standard 3D rectangular border
			if (m_bBorder) pDC->DrawEdge(&r, EDGE_RAISED, BF_RECT);
		}
		// paint the focus rect
		if ((lpDrawItemStruct->itemState & ODS_FOCUS) && (m_FocusRectMargin > 0)) {
			r.left   += m_FocusRectMargin;
			r.top    += m_FocusRectMargin;
			r.right  -= m_FocusRectMargin;
			r.bottom -= m_FocusRectMargin;
			DrawFocusRect(lpDrawItemStruct->hDC, &r);
		}
		// paint the enabled button text
		pDC->SetTextColor(m_TextColor);
		pDC->ExtTextOut(tx, ty, 0, &r, sCaption, NULL); // tx, ty ETO_CLIPPED
	}
}

void CxSkinButton::DrawBitmap(CDC *dc, HBITMAP hbmp, RECT r, int DrawMode)
{
	//	DrawMode: 0 = Normal; 1 = stretch; 2 = tiled fill
	if (DrawMode == 2) {
		FillWithBitmap(dc, hbmp, r);
		return;
	}
	if (!hbmp) {
		return;
	}

	int cx = r.right - r.left;
	int cy = r.bottom - r.top;
	CDC dcBmp, dcMask;
	dcBmp.CreateCompatibleDC(dc);
	dcBmp.SelectObject(hbmp);

	if (m_bmpMask.m_hObject != NULL) {
		dcMask.CreateCompatibleDC(dc);
		dcMask.SelectObject(m_bmpMask);

		CDC hdcMem;
		hdcMem.CreateCompatibleDC(dc);
		CBitmap hBitmap;
		hBitmap.CreateCompatibleBitmap(dc, cx, cy);
		hdcMem.SelectObject(hBitmap);

		hdcMem.BitBlt(r.left, r.top, cx, cy, dc, 0, 0, SRCCOPY);
		if (!DrawMode){
			hdcMem.BitBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, SRCINVERT);
			hdcMem.BitBlt(r.left, r.top, cx, cy, &dcMask, 0, 0, SRCAND);
			hdcMem.BitBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, SRCINVERT);
		} else {
			int bx = ctrlExGetBitmapWidth(hbmp);
			int by = ctrlExGetBitmapHeight(hbmp);
			hdcMem.StretchBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, bx, by, SRCINVERT);
			hdcMem.StretchBlt(r.left, r.top, cx, cy, &dcMask, 0, 0, bx, by, SRCAND);
			hdcMem.StretchBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, bx, by, SRCINVERT);
		}
		dc->BitBlt(r.left, r.top, cx, cy, &hdcMem, 0, 0, SRCCOPY);

		hBitmap.DeleteObject();

	} else {
		if (!DrawMode){
			dc->BitBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, SRCCOPY);
		} else {
			int bx = ctrlExGetBitmapWidth(hbmp);
			int by = ctrlExGetBitmapHeight(hbmp);
			dc->StretchBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, bx, by, SRCCOPY);
		}
	}
}

void CxSkinButton::FillWithBitmap(CDC *dc, HBITMAP hbmp, RECT r)
{
	if (!hbmp) {
		return;
	}

	CDC memdc;
	memdc.CreateCompatibleDC(dc);
	memdc.SelectObject(hbmp);
	int w = r.right - r.left;
	int	h = r.bottom - r.top;
	int x, y, z;
	int	bx = ctrlExGetBitmapWidth(hbmp);
	int	by = ctrlExGetBitmapHeight(hbmp);
	for (y = r.top; y < h; y += by) {
		if ((y + by) > h) {
			by = h - y;
		}
		z = bx;
		for (x = r.left ; x < w ; x += z) {
			if ((x + z) > w) z = w - x;
			dc->BitBlt(x, y, z, by, &memdc, 0, 0, SRCCOPY);
		}
	}
}

void CxSkinButton::SetSkin(UINT normal, UINT down, UINT over, UINT disabled, UINT off, UINT mask,
						   short drawmode, short border, UINT cursorID, short margin)
{
	m_bmpNormal.DeleteObject();
	m_bmpDown.DeleteObject();
	m_bmpOver.DeleteObject();
	m_bmpDisabled.DeleteObject();
	m_bmpMask.DeleteObject();
	m_bmpOff.DeleteObject();

	if (normal > 0) m_bmpNormal.LoadBitmap(normal);
	if (down > 0) m_bmpDown.LoadBitmap(down);
	if (over > 0) m_bmpOver.LoadBitmap(over);


	if (disabled > 0) m_bmpDisabled.LoadBitmap(disabled);
	else if (normal > 0) m_bmpDisabled.LoadBitmap(normal);

	if (off > 0) m_bmpOff.LoadBitmap(off);
	else if (normal > 0) m_bmpOff.LoadBitmap(normal);

	m_DrawMode = max(0, min(drawmode, 2));
	m_bBorder = border;
	m_FocusRectMargin = max(0, margin);

	if (mask>0) {
		m_bmpMask.LoadBitmap(mask);
		if (m_hClipRgn) {
			DeleteObject(m_hClipRgn);
		}
		m_hClipRgn = CreateRgnFromBitmap(m_bmpMask, RGB(255, 255, 255));
		if (m_hClipRgn) {
			SetWindowRgn(m_hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(), m_hClipRgn);
		}
		if (m_DrawMode == 0) {
			SetWindowPos(NULL, 0, 0, ctrlExGetBitmapWidth(m_bmpMask),
				ctrlExGetBitmapHeight(m_bmpMask), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
	if (m_DrawMode == 2) {
		CDC *pDC = GetDC();
		CString sCaption;
		GetWindowText(sCaption);
		// CDC *pDC = CDC::FromHandle(::GetDC(m_hwnd));	// get device context
#if USE_TEXT_METRIC
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);							// get font metrics
#else
		CSize cs = pDC->GetTextExtent(sCaption);
#endif
		CRect r;
		GetWindowRect(&r);
		int cx = r.right - r.left;
		int cy = r.bottom - r.top;

		if (m_DrawMode == 2 && sCaption.GetLength() != 0) {
#if USE_TEXT_METRIC
			int textw = tm.tmAveCharWidth * sCaption.GetLength();
			int texth = tm.tmHeight;
#else
			int textw = cs.cx;
			int texth = cs.cy;
#endif
			SetWindowPos(NULL, 0, 0, max(cx, textw),
				cy + texth, SWP_NOZORDER | SWP_NOMOVE);
		}
	}
	if (cursorID != 0) {
		SetBtnCursor(cursorID);
	}
}
void CxSkinButton::SetSkin(HBITMAP normal, HBITMAP down, HBITMAP over, HBITMAP disabled,
						   HBITMAP off, HBITMAP off_on, HBITMAP mask, short drawmode, short border, UINT cursorID, short margin)
{
	m_bmpNormal.DeleteObject();
	m_bmpDown.DeleteObject();
	m_bmpOver.DeleteObject();
	m_bmpDisabled.DeleteObject();
	m_bmpMask.DeleteObject();
	m_bmpOff.DeleteObject();
	m_bmpOffOn.DeleteObject();

	if (normal) m_bmpNormal.Attach(normal);
	if (down) m_bmpDown.Attach(down);
	if (over) m_bmpOver.Attach(over);
	if (disabled) m_bmpDisabled.Attach(disabled);

	if (off) m_bmpOff.Attach(off);
	else m_bmpOff.Attach(normal);

	if (off_on) m_bmpOffOn.Attach(off_on);
	// else m_bmpOffOn.Attach(over);

	m_DrawMode = max(0, min(drawmode, 2));
	m_bBorder = border;
	m_FocusRectMargin = max(0, margin);

	if (mask > 0) {
		m_bmpMask.Attach(mask);
		if (m_hClipRgn) {
			DeleteObject(m_hClipRgn);
		}
		m_hClipRgn = CreateRgnFromBitmap(m_bmpMask, RGB(255, 255, 255));
		if (m_hClipRgn) {
			SetWindowRgn(m_hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(), m_hClipRgn);
		}
		if (m_DrawMode == 0) {
			SetWindowPos(NULL, 0, 0, ctrlExGetBitmapWidth(m_bmpMask),
				ctrlExGetBitmapHeight(m_bmpMask), SWP_NOZORDER | SWP_NOMOVE);
		}
	}

	if (cursorID != 0) {
		SetBtnCursor(cursorID);
	}
}
void CxSkinButton::SetSkin(CBitmap *normal, CBitmap *down, CBitmap *over, 
						   CBitmap *disabled, CBitmap *off, UINT mask, short drawmode,
						   short border, UINT cursorID, short margin)
{
	m_bmpNormal.DeleteObject();
	m_bmpDown.DeleteObject();
	m_bmpOver.DeleteObject();
	m_bmpDisabled.DeleteObject();
	m_bmpMask.DeleteObject();
	m_bmpOff.DeleteObject();

	BITMAP bmp;
	if (normal) {
		normal->GetBitmap(&bmp);
		ExtractBitmap(normal, &m_bmpNormal, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	}
	if (down) {
		down->GetBitmap(&bmp);
		ExtractBitmap(down, &m_bmpDown, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	}
	if (over) {
		over->GetBitmap(&bmp);
		ExtractBitmap(over, &m_bmpOver, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	}

	if (disabled) {
		disabled->GetBitmap(&bmp);
		ExtractBitmap(disabled, &m_bmpDisabled, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	} else if (normal) {
		normal->GetBitmap(&bmp);
		ExtractBitmap(normal, &m_bmpDisabled, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	}

	if (off) {
		off->GetBitmap(&bmp);
		ExtractBitmap(off, &m_bmpOff, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	} else if (normal) {
		normal->GetBitmap(&bmp);
		ExtractBitmap(normal, &m_bmpOff, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
	}

	m_DrawMode = max(0, min(drawmode, 2));
	m_bBorder = border;
	m_FocusRectMargin = max(0, margin);

	if (mask > 0) {
		m_bmpMask.LoadBitmap(mask);
		if (m_hClipRgn) {
			DeleteObject(m_hClipRgn);
		}
		m_hClipRgn = CreateRgnFromBitmap(m_bmpMask, RGB(255, 255, 255));
		if (m_hClipRgn) {
			SetWindowRgn(m_hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(), m_hClipRgn);
		}
		if (m_DrawMode == 0) {
			SetWindowPos(NULL, 0, 0, ctrlExGetBitmapWidth(m_bmpMask),
				ctrlExGetBitmapHeight(m_bmpMask), SWP_NOZORDER | SWP_NOMOVE);
		}
	}

	if (cursorID != 0) {
		SetBtnCursor(cursorID);
	}
}

void CxSkinButton::setButtonSkin(LPCTSTR folderName, LPCTSTR btnName)
{
	TCHAR optkey[_MAX_PATH];

	SetWindowText(_T(""));
	ModifyStyle(0, BS_OWNERDRAW, 0);
	
	_stprintf_s(optkey, _T("%s_%s"), _T("N"), btnName);
	HBITMAP hbmp = ctrlExGetConfigBitmap(folderName, optkey);
	ASSERT(hbmp != NULL);

	HBITMAP hbmp_down = NULL;
	HBITMAP hbmp_on = NULL;
	HBITMAP hbmp_disable = NULL;
	HBITMAP hbmp_off = NULL;
	HBITMAP hbmp_off_on = NULL;

	_stprintf_s(optkey, _T("%s_%s"), _T("D"), btnName);
	hbmp_down = ctrlExGetConfigBitmap(folderName, optkey);

	_stprintf_s(optkey, _T("%s_%s"), _T("H"), btnName);
	hbmp_on = ctrlExGetConfigBitmap(folderName, optkey);

	_stprintf_s(optkey, _T("%s_%s"), _T("Disable"), btnName);
	hbmp_disable = ctrlExGetConfigBitmap(folderName, optkey);

	_stprintf_s(optkey, _T("%s_%s"), _T("Off"), btnName);
	hbmp_off = ctrlExGetConfigBitmap(folderName, optkey);

	_stprintf_s(optkey, _T("%s_%s"), _T("OffOn"), btnName);
	hbmp_off_on = ctrlExGetConfigBitmap(folderName, optkey);

	int width = ctrlExGetBitmapWidth(hbmp);
	int height = ctrlExGetBitmapHeight(hbmp);
	SetWindowPos(0, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);

	hbmp_down = hbmp_down == NULL ? hbmp : hbmp_down;
	hbmp_on = hbmp_on == NULL ? hbmp : hbmp_on;
	hbmp_disable = hbmp_disable == NULL ? hbmp : hbmp_disable;
	hbmp_off = hbmp_off == NULL ? hbmp : hbmp_off;
	hbmp_off_on = hbmp_off_on == NULL ? hbmp : hbmp_off_on;
	SetSkin(hbmp, hbmp_down, hbmp_on, hbmp_disable, hbmp_off, hbmp_off_on);
}

HRGN CxSkinButton::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	if (!hBmp) return NULL;

	BITMAP bm;
	GetObject(hBmp, sizeof(BITMAP), &bm);

	CDC dcBmp;
	dcBmp.CreateCompatibleDC(GetDC());
	dcBmp.SelectObject(hBmp);

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
	const DWORD MAXBUF = 40;	// size of one block in RECTs
	// (i.e. MAXBUF * sizeof(RECT) in bytes)
	LPRECT pRects;								
	DWORD cBlocks = 0;			// number of allocated blocks

	INT i, j;					// current position in mask image
	INT first = 0;				// left position of current scan line
	bool wasfirst = false;		// set when if mask was found in current scan line
	bool ismask;				// set when current color is mask color

	// allocate memory for region data
	RGNDATAHEADER *pRgnData = (RGNDATAHEADER *)new BYTE[RDHDR + ++cBlocks * MAXBUF * sizeof(RECT)];
	memset(pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT));
	// fill it by default
	pRgnData->dwSize = RDHDR;
	pRgnData->iType = RDH_RECTANGLES;
	pRgnData->nCount = 0;
	for (i = 0; i < bm.bmHeight; i++) {
		for (j = 0; j < bm.bmWidth; j++) {
			// get color
			ismask = (dcBmp.GetPixel(j, bm.bmHeight - i - 1) != color);
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if (wasfirst && (ismask ^ (j < bm.bmWidth - 1))) {
				// get offset to RECT array if RGNDATA buffer
				pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
				// save current RECT
				pRects[pRgnData->nCount++] = CRect(first, bm.bmHeight - i - 1, j, bm.bmHeight - i);
				// if buffer full reallocate it
				if (pRgnData->nCount >= cBlocks * MAXBUF) {
					LPBYTE pRgnDataNew = new BYTE[RDHDR + ++cBlocks * MAXBUF * sizeof(RECT)];
					memcpy(pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT));
					delete pRgnData;
					pRgnData = (RGNDATAHEADER *)pRgnDataNew;
				}
				wasfirst = false;
			} else if (!wasfirst && ismask) {		// set wasfirst when mask is found
				first = j;
				wasfirst = true;
			}
		}
	}
	DeleteObject(dcBmp); //release the bitmap
	// create region
	// Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net)
	// HRGN hRgn = ExtCreateRegion(NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData);
	// ExtCreateRegion replacement {
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);

	pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
	for (i = 0; i < (int)pRgnData->nCount; i++) {
		HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
		VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR) != ERROR);
	}
	ASSERT( hRgn!=NULL );
	// } ExtCreateRegion replacement

	delete pRgnData;
	return hRgn;
}

COLORREF CxSkinButton::SetTextColor(COLORREF newColor)
{
	COLORREF tmpColor = m_TextColor;
	m_TextColor = newColor;
	return tmpColor;
}

void CxSkinButton::SetToolTipText(CString s)
{
	if (m_tooltip.m_hWnd == NULL) {
		if (m_tooltip.Create(this))	{
			m_tooltip.SetTipBkColor(RGB(255, 255, 255));
			m_tooltip.SetMaxTipWidth(200);
			if (m_tooltip.AddTool(this, (LPCTSTR)s)) {
				m_strToolTip = s;
				m_tooltip.Activate(1);
			}
		}
	} else {
		m_strToolTip = s;
		m_tooltip.UpdateTipText((LPCTSTR)s, this);
	}
}

void CxSkinButton::SetOffState(BOOL newstate)
{
	m_OffState = newstate;
	Invalidate(); 
}

void CxSkinButton::SetOffSkin(UINT off)
{
	if (NULL == off) return;
	m_bmpOff.DeleteObject();
	m_bmpOff.LoadBitmap(off);
}

void CxSkinButton::SetOffSkin(CBitmap *off)
{
	if (NULL == off) return;
	m_bmpOff.DeleteObject();
	BITMAP bmp;
	off->GetBitmap(&bmp);
	ExtractBitmap(off, &m_bmpOff, CRect(0, 0, bmp.bmWidth, bmp.bmHeight));
}

void CxSkinButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// This function will create a MSG structure, fill it in a pass it to
	// the ToolTip control, m_ttip.  Note that we ensure the point is in window
	// coordinates (relative to the control's window).
	if (NULL != m_tooltip.m_hWnd) {
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.UpdateTipText(m_strToolTip, this);
		m_tooltip.RelayEvent(&msg);
	}
}

void CxSkinButton::OnLButtonDblClk(UINT flags, CPoint point) 
{
	SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(point.x, point.y));

	CButton::OnLButtonDblClk( flags, point );
}

void CxSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	RelayEvent(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(LOWORD(point.x), LOWORD(point.y)));

	if (m_tooltip.GetSafeHwnd() != NULL) {
		m_tooltip.Activate(FALSE);
	}
	//If we are tracking this button, cancel it
	if (m_tracking) {
		TRACKMOUSEEVENT t = {
			sizeof(TRACKMOUSEEVENT),
			TME_CANCEL | TME_LEAVE,
			m_hWnd,
			0
		};
		if (::_TrackMouseEvent(&t)) {
			m_tracking = false;
		}
	}
	//Default-process the message
	m_button_down = true;
	CButton::OnLButtonDown(nFlags, point);
}

void CxSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	RelayEvent(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(LOWORD(point.x), LOWORD(point.y)));

	if (m_tooltip.GetSafeHwnd() != NULL) {
		m_tooltip.Activate(TRUE);
	}
	//Default-process the message
	m_button_down = false;
	CButton::OnLButtonUp(nFlags, point);
}

void CxSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

	//If we are in capture mode, button has been pressed down
	//recently and not yet released - therefore check is we are
	//actually over the button or somewhere else. If the mouse
	//position changed considerably (e.g. we moved mouse pointer
	//from the button to some other place outside button area)
	//force the control to redraw
	//
	if (m_button_down && (::GetCapture() == m_hWnd)) {
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);

		bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
		bool need_pressed = (mouse_wnd == m_hWnd);
		if (pressed != need_pressed) {
			SetState(need_pressed ? TRUE : FALSE);
			Invalidate();
		}
	} else {

		//Otherwise the button is released. That means we should
		//know when we leave its area - and so if we are not tracking
		//this mouse leave event yet, start now!
		//
		if (!m_tracking) {
			TRACKMOUSEEVENT t = {
				sizeof(TRACKMOUSEEVENT),
				TME_LEAVE,
				m_hWnd,
				0
			};
			if (::_TrackMouseEvent(&t)) {
				//TRACE("* Mouse enter\n");
				m_tracking = true;
				Invalidate();
			}
		}
	}

	//Forward this event to superclass
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CxSkinButton::OnMouseLeave(WPARAM, LPARAM)
{
	ASSERT (m_tracking);
	//TRACE("* Mouse leave\n");
	m_tracking = false;
	Invalidate();
	return 0;
}

void CxSkinButton::OnKillFocus(CWnd *new_wnd)
{
	if (::GetCapture() == m_hWnd) {
		::ReleaseCapture();
		ASSERT (!m_tracking);
		m_button_down = false;
	}
	CButton::OnKillFocus(new_wnd);
}

BOOL CxSkinButton::OnClicked() 
{
	if (::GetCapture() == m_hWnd) {
		::ReleaseCapture();
		ASSERT (!m_tracking);
	}
	m_button_down = false;
	//Invalidate();
	return FALSE;
}

BOOL CxSkinButton::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor != NULL) {
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CxSkinButton::SetBtnCursor(UINT cursorID)
{
	HINSTANCE hInstResource;
	// Destroy any previous cursor
	if (m_hCursor != NULL) ::DestroyCursor(m_hCursor);
	m_hCursor = NULL;

	// If we want a cursor
	if (cursorID != 0) {
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(cursorID),
			RT_GROUP_CURSOR);
		// Load icon resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource/*AfxGetApp()->m_hInstance*/, MAKEINTRESOURCE(cursorID), IMAGE_CURSOR, 0, 0, 0);
		// If something wrong then return FALSE
		if (m_hCursor == NULL) return FALSE;
	}

	return TRUE;
}


