/////////////////////////////////////////////
// CMenuEx - Version 1.1 - August 30, 2004 //
// --------------------------------------- //
// MFC-derived class for owner-drawn and   //
// Office XP style menus.                  //
// --------------------------------------- //
// Copyright (C) 2004 by C. Buegenburg     //
// All rights reserved.                    //
// --------------------------------------- //
// You are permitted to freely use and     //
// distribute this class, as long as you   //
// a) give proper credit to me and         //
// b) do not remove my copyright notes and //
// c) report any code changes to me.       //
/////////////////////////////////////////////

#include "stdafx.h"
#include "MenuEx.h"

////////////////////////////
// Statics Implementation //
////////////////////////////

COLORREF	CMenuEx::m_colHead;			// Default: COLOR_MENUBAR
COLORREF	CMenuEx::m_colBackground;	// Default: COLOR_MENU
COLORREF	CMenuEx::m_colHorzBar;		// Default: COLOR_MENUBAR
COLORREF	CMenuEx::m_colText;			// Default: COLOR_MENUTEXT
COLORREF    CMenuEx::m_colHilightText;
COLORREF	CMenuEx::m_colGrayText;		// Default: COLOR_GRAYTEXT
COLORREF	CMenuEx::m_colSeparator;	// Default: COLOR_MENUBAR
COLORREF	CMenuEx::m_colHilightBorder;// Default: COLOR_3DSHADOW
COLORREF	CMenuEx::m_colHilight;		// Default: COLOR_MENUHILIGHT
COLORREF	CMenuEx::m_colGrayHilight;	// Default: COLOR_MENU
BOOL		CMenuEx::m_bHighlightGray;	// Default: FALSE
CSize		CMenuEx::m_szImage;			// Default: 16x16
CSize		CMenuEx::m_szImagePadding;	// Default: 3x3
CSize		CMenuEx::m_szTextPadding;	// Default: 3x3

////////////////////////////////
// Construction & Destruction //
////////////////////////////////

CMenuEx::CMenuEx(CWnd* pWnd)
{
	CMenu::CMenu();
	m_pWnd = pWnd;

	// Defaults
/*	DWORD dwVersion		= GetVersion();
	if(LOBYTE(LOWORD(dwVersion)) >= 5)
	{
*/		m_colHead		= GetSysColor(COLOR_MENUBAR);
		m_colHilight	=  RGB(203,203,203) ;//GetSysColor(COLOR_MENUHILIGHT	);
/*	}
	else
	{
		m_colHead		= GetSysColor(COLOR_3DFACE		);
		m_colHilight	=  RGB(203,203,203) ;//GetSysColor(COLOR_HIGHLIGHT	);
	}
*/
//	m_colHilight        = RGB(209,232,249);
	m_colBackground		= RGB(234,234,234);//GetSysColor(COLOR_MENU		);
	m_colHorzBar		= RGB(111,117,122);//GetSysColor(COLOR_MENUBAR		);
	m_colText			= RGB(0,0,0);/*GetSysColor(COLOR_MENUTEXT	);*/
	m_colHilightText    = RGB(0,0,0);
	m_colGrayText		= GetSysColor(COLOR_GRAYTEXT	);
	m_colSeparator		= RGB(200,200,200);//GetSysColor(COLOR_MENUBAR		);
	m_colHilightBorder	= RGB(175,175,175);//GetSysColor(COLOR_3DSHADOW	);
	m_colGrayHilight	=  RGB(255,0,0);GetSysColor(COLOR_MENU		);
	m_bHighlightGray	= FALSE;
	m_szImage			= CSize(16, 16);
	m_szImagePadding	= CSize(3, 3);
	m_szTextPadding		= CSize(5, 3);
}

CMenuEx::~CMenuEx()
{
	CMenu::~CMenu();

	for(INT32 a=0; a<m_TempMenus.GetSize(); a++)
	{
		delete (CMenuEx*) m_TempMenus[a];
	}
	for(INT32 b=0; b<m_TempItems.GetSize(); b++)
	{
		delete (MENUITEMEX*) m_TempItems[b];
	}
	for(INT32 c=0; c<m_ImageLists.GetSize(); c++)
	{
		delete (CImageList*) m_ImageLists[c];
	}
}

////////////////////
// Public Methods //
////////////////////

BOOL CMenuEx::CreatePopupMenu()
{
  if(m_pWnd == NULL)
  {
	  m_pWnd = AfxGetMainWnd();
  }

   return CMenu::CreatePopupMenu();
}
BOOL CMenuEx::LoadMenu(UINT nIDResource)
{
	BOOL bResult = CMenu::LoadMenu(nIDResource);
	if(bResult) MakeOwnerDrawn(TRUE);
	return bResult;
}

BOOL CMenuEx::LoadMenu(LPCTSTR lpszResourceName)
{
	BOOL bResult = CMenu::LoadMenu(lpszResourceName);
	if(bResult) MakeOwnerDrawn(TRUE);
	return bResult;
}

BOOL CMenuEx::Attach(HMENU hMenu)
{
	BOOL bResult = CMenu::Attach(hMenu);
	if(bResult) MakeOwnerDrawn(FALSE);
	return bResult;
}

void CMenuEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	MENUITEMEX* pItem = (MENUITEMEX*) lpDIS->itemData;
     
	RECT rcOld = lpDIS->rcItem;

	if(pItem->m_uiID != 0 )
	if(lpDIS->rcItem.bottom - lpDIS->rcItem.top < 22)
	{
      lpDIS->rcItem.bottom += 6;
	}

	// Define Rectangles
	CRect rectBar	(lpDIS->rcItem);
	CRect rectImage	(lpDIS->rcItem);
	CRect rectItem	(lpDIS->rcItem);
	CRect rectText	(lpDIS->rcItem);

	if(pItem->m_bIsTopLevel)
	{
		rectBar	= CRect(0, 0, 0, 0);
		rectText.left	+= 2;
		rectText.top	+= 3;
	}
	else
	{
		rectBar	.right	 = rectBar.left+m_szImagePadding.cx+m_szImage.cx+m_szImagePadding.cx;
		rectImage.left	+= m_szImagePadding.cx;
		rectImage.top	+= m_szImagePadding.cy;
		rectImage.right	 = rectImage.left+m_szImage.cx;
		rectImage.bottom = rectImage.top +m_szImage.cy;
		rectItem.left	+= m_szImagePadding.cx+m_szImage.cx+m_szImagePadding.cx;
		rectText.left	+= m_szImagePadding.cx+m_szImage.cx+m_szImagePadding.cx+m_szTextPadding.cx;
		rectText.top	+= m_szTextPadding.cy;

	}

	// Color Bar
	pDC->FillSolidRect(&rectBar, m_colHorzBar);

	// Item
	if(pItem->m_uiID == 0 && !pItem->m_bNewSepartor)
	{
		// Separator
		pDC->FillSolidRect(&rectItem, m_colBackground);
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, m_colSeparator);
		CPen* pOldPen = pDC->SelectObject(&pen);
		//pDC->MoveTo(rectItem.left+m_szTextPadding.cx,  rectItem.top+(rectItem.bottom-rectItem.top)/2);
		pDC->MoveTo(rectItem.left,  rectItem.top+(rectItem.bottom-rectItem.top)/2);
		pDC->LineTo(rectItem.right, rectItem.top+(rectItem.bottom-rectItem.top)/2);
		pDC->SelectObject(pOldPen);
		DeleteObject(pen);
	}
	else if(pItem->m_uiID == 0 && pItem->m_bNewSepartor)
	{
       pDC->FillSolidRect(&rectItem, m_colBackground);
	}
	else if((lpDIS->itemState	& ODS_SELECTED)						&&
			(!(lpDIS->itemState	& ODS_GRAYED) || m_bHighlightGray)	&&						
			(lpDIS->itemAction	& (ODA_SELECT|ODA_DRAWENTIRE)))
	{
		// Selected Item
		CPen	pen;
		CBrush	brush;
		if(lpDIS->itemState & ODS_GRAYED)
		{
			pDC->SetTextColor(m_colGrayText);
			pen		.CreatePen			(PS_SOLID, 1, m_colHilightBorder);
			brush	.CreateSolidBrush	(m_colGrayHilight);
		}
		else
		{
			pDC->SetTextColor(m_colHilightText);
			pen		.CreatePen			(PS_SOLID, 1,  m_colHilightBorder);
			brush	.CreateSolidBrush	(m_colHilight);
		}

		CPen*	pOldPen		= pDC->SelectObject(&pen  );
		CBrush*	pOldBrush	= pDC->SelectObject(&brush);
		pDC->Rectangle(&lpDIS->rcItem);
		pDC->SelectObject(pOldPen  );
		pDC->SelectObject(pOldBrush);
		DeleteObject(pen);
		DeleteObject(brush);
	}
	else
	{
		// Grayed or Normal Item
		if(lpDIS->itemState & ODS_GRAYED)
			pDC->SetTextColor(m_colGrayText);
		else
			pDC->SetTextColor(m_colText);
		if(pItem->m_bIsTopLevel)
			pDC->FillSolidRect(&rectItem, m_colHead);
		else
			pDC->FillSolidRect(&rectItem, m_colBackground);
	}

	if(lpDIS->itemState & ODS_CHECKED)
	{
		// Checked Item
		CPen		pen;
		CPen*		pOldPen;
		CBrush		brush;
		CBrush*		pOldBrush;
		HBITMAP		hBmp;
		CBitmap*	pBmp;
		BITMAP		bmp;
		CSize		szBmp;
		CPoint		ptBmp;
		ZeroMemory(&bmp, sizeof(BITMAP));
		
		pen			.CreatePen(PS_SOLID, 1, m_colHilightBorder);
		brush		.CreateSolidBrush(m_colHorzBar);
		pOldPen		= pDC->SelectObject(&pen  );
		pOldBrush	= pDC->SelectObject(&brush);
		hBmp		= LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CHECK));
		pBmp		= CBitmap::FromHandle(hBmp);
		pBmp		->GetBitmap(&bmp);
		szBmp		= CSize(bmp.bmWidth, bmp.bmHeight);
		ptBmp		= CPoint(rectImage.left+(m_szImage.cx-szBmp.cx)/2+1, rectImage.top+(m_szImage.cy-szBmp.cy)/2);
		pDC->SetBkColor		(m_colHorzBar);
		pDC->Rectangle		(rectImage.left, rectImage.top, rectImage.right, rectImage.bottom);
		pDC->DrawState		(ptBmp, szBmp, hBmp, DSS_NORMAL);
		pDC->SelectObject	(pOldPen  );
		pDC->SelectObject	(pOldBrush);
		DeleteObject(pen  );
		DeleteObject(brush);
		DeleteObject(hBmp );
	}
	else if((pItem->m_nImageEnabled >= 0) || (pItem->m_nImageDisabled >= 0))
	{
		// Item with Image
		if((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemState & ODS_GRAYED) && m_bHighlightGray)
			pDC->SetBkColor(m_colGrayHilight);
		else
			pDC->SetBkColor(m_colHorzBar);

		CImageList* pilEnabled	= (CImageList*) m_ImageLists[pItem->m_nListEnabled];
		CImageList* pilDisabled	= (CImageList*) m_ImageLists[pItem->m_nListDisabled];
		if(lpDIS->itemState & ODS_GRAYED)
		{
			// Grayed Item
			if(pilDisabled)
			{
				// Use Disabled ImageList
				pilDisabled->Draw(pDC, pItem->m_nImageDisabled, CPoint(rectImage.left, rectImage.top), ILD_TRANSPARENT);
			}
			else
			{
				// Use Enabled ImageList
				pilEnabled->Draw(pDC, pItem->m_nImageEnabled, CPoint(rectImage.left, rectImage.top), ILD_MASK);
			}
		}
		else
		{
			// Normal Item
			pilEnabled->Draw(pDC, pItem->m_nImageEnabled, CPoint(rectImage.left, rectImage.top), ILD_TRANSPARENT);
		}
	}

	// Text
	pDC->SetBkMode(TRANSPARENT);
	if(pItem->m_bIsTopLevel)
		pDC->DrawText(pItem->m_sCaption, &rectText, DT_EXPANDTABS|DT_CENTER|DT_VCENTER);
	else
		pDC->DrawText(pItem->m_sCaption, &rectText, DT_EXPANDTABS|DT_LEFT|DT_VCENTER);

//	if( /*(lpDIS->itemState & ODS_GRAYED) && */GetSubMenu(pItem->m_uiID) != NULL)
	if(pItem->m_bHasSubMenu)
	{
		RECT arrow  = rcOld;
		arrow.left  = arrow.right-10;
		arrow.top += 3;
		arrow.bottom -= 3;
		DrawMenuArrow(pDC->GetSafeHdc(),arrow, !(lpDIS->itemState&ODS_GRAYED));
	
		pDC->ExcludeClipRect( &rcOld);
		
	}

}

void CMenuEx::DrawMenuArrow(HDC inHDC, RECT &inDestR,
							 bool inIsEnabled)
{
	//Create the DCs and Bitmaps we will need
	HDC arrowDC = ::CreateCompatibleDC(inHDC);
	HDC fillDC  = ::CreateCompatibleDC(inHDC);
	int arrowW  = inDestR.right - inDestR.left;
	int arrowH  = inDestR.bottom - inDestR.top;
	HBITMAP arrowBitmap    = ::CreateCompatibleBitmap(inHDC, arrowW, arrowH);
	HBITMAP oldArrowBitmap = (HBITMAP)::SelectObject(arrowDC, arrowBitmap);
	HBITMAP fillBitmap     = ::CreateCompatibleBitmap(inHDC, arrowW, arrowH);
	HBITMAP oldFillBitmap  = (HBITMAP)::SelectObject(fillDC, fillBitmap);
	
	//Set the offscreen arrow rect
	RECT tmpArrowR;
	::SetRect(&tmpArrowR, 0, 0, arrowW, arrowH);
	
	//Draw the frame control arrow (The OS draws this as a black on
	//                              white bitmap mask)
	::DrawFrameControl(arrowDC, &tmpArrowR, DFC_MENU, DFCS_MENUARROW);
	
	//Set the arrow color
		HBRUSH arrowBrush = inIsEnabled ? ::GetSysColorBrush(COLOR_MENUTEXT) :
		::GetSysColorBrush(COLOR_GRAYTEXT);
	//HBRUSH arrowBrush = CreateSolidBrush(RGB(0,0,255));
	//	::FillRect(hDC,&tmpArrowR,H);
	
	//Fill the fill bitmap with the arrow color
	::FillRect(fillDC, &tmpArrowR, arrowBrush);
	
	//Blit the items in a masking fashion
	::BitBlt(inHDC, inDestR.left, inDestR.top, arrowW, arrowH, fillDC,
		0, 0, SRCINVERT);
	::BitBlt(inHDC, inDestR.left, inDestR.top, arrowW, arrowH, arrowDC,
		0, 0, SRCAND);
	::BitBlt(inHDC, inDestR.left, inDestR.top, arrowW, arrowH, fillDC,
		0, 0, SRCINVERT);
	
	//Clean up
	::SelectObject(fillDC, oldFillBitmap);
	::DeleteObject(fillBitmap);
	::SelectObject(arrowDC, oldArrowBitmap);
	::DeleteObject(arrowBitmap);
	::DeleteDC(fillDC);
	::DeleteDC(arrowDC);
	//::DeleteObject(arrowBrush);
}

void CMenuEx::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	OnMeasureItem(lpMIS, m_pWnd);
}

void CMenuEx::OnMeasureItem(LPMEASUREITEMSTRUCT lpMIS, CWnd* pWnd)
{
	MENUITEMEX* pItem = (MENUITEMEX*) lpMIS->itemData;

	CDC* pDC = pWnd->GetDC();
	CFont* pOldFont = pDC->SelectObject(pWnd->GetFont());
	if(pItem->m_bIsTopLevel)
	{
		lpMIS->itemWidth	= pItem->m_sCaption.GetLength()*5;
		lpMIS->itemHeight	= 20;
	}
	else
	{
		lpMIS->itemWidth	= m_szImagePadding.cx+m_szImage.cx+m_szImagePadding.cx+m_szTextPadding.cx+pDC->GetTextExtent(pItem->m_sCaption).cx+m_szTextPadding.cx;
		lpMIS->itemHeight	= m_szTextPadding.cy+pDC->GetTextExtent(pItem->m_sCaption).cy+m_szTextPadding.cy;
		if(pItem->m_uiID == 0 && !pItem->m_bNewSepartor)
			//lpMIS->itemHeight = m_szTextPadding.cy+1+m_szTextPadding.cy;
			lpMIS->itemHeight = m_szTextPadding.cy/2;
		else if(pItem->m_uiID == 0 && pItem->m_bNewSepartor)
			lpMIS->itemHeight = 6;
		else if(lpMIS->itemHeight < (UINT)(m_szImagePadding.cy+m_szImage.cy+m_szImagePadding.cy))
		{
			UINT nNewHeight = (UINT)(m_szImagePadding.cy+m_szImage.cy+m_szImagePadding.cy);
			//UINT nDiff		= nNewHeight-lpMIS->itemHeight;
			//m_szTextPadding.cy += nDiff/2;
			lpMIS->itemHeight = nNewHeight;
		}
	}


	pDC->SelectObject(pOldFont);
	pWnd->ReleaseDC(pDC);
}

void CMenuEx::UseToolBarImages(CToolBar* pToolBar)
{
	UINT	uiID;
	UINT	uiStyle;
	INT		nImage;
	for(INT nItem=0; nItem<pToolBar->GetCount(); nItem++)
	{
		pToolBar->GetButtonInfo(nItem, uiID, uiStyle, nImage);
		if(uiStyle != TBBS_BUTTON) continue;
		SetItemImages(uiID, nImage, pToolBar);
	}
}

void   CMenuEx::SetOwnerDrawn()
{
   MakeOwnerDrawn(FALSE);
}

BOOL   CMenuEx::ModifyMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem)
{
  ASSERT(::IsMenu(m_hMenu)); 
  return ::ModifyMenu(m_hMenu, nPosition, nFlags, nIDNewItem, lpszNewItem); 
   
 /* if( (nFlags&MF_BYPOSITION) == MF_BYPOSITION)
  {
    MENUITEMEX* pItem  = (MENUITEMEX*)m_TempItems.GetAt(nPosition);
    pItem->m_uiID = nIDNewItem;
	pItem->m_sCaption = lpszNewItem;
	return	CMenu::ModifyMenu(nPosition, MF_BYPOSITION|MF_OWNERDRAW, pItem->m_uiID, (TCHAR*) pItem);
  }
  else
  {
	  for(int i = 0; i <m_TempItems.GetSize(); i++)
	  {
		  MENUITEMEX* pItem  = (MENUITEMEX*)m_TempItems.GetAt(i);
		  
		  if(pItem->m_uiID == nPosition)
		  {
			  pItem->m_uiID = nIDNewItem;
	          pItem->m_sCaption = lpszNewItem;
			  return	CMenu::ModifyMenu(nPosition, MF_BYCOMMAND|MF_OWNERDRAW, pItem->m_uiID, (TCHAR*) pItem);
		  }
	  }
  }

  return TRUE;*/
}

///////////////////////
// Protected Methods //
///////////////////////

void CMenuEx::MakeOwnerDrawn(BOOL bTopLevel)
{
	CMenu*	pSubMenu;
	for(int ui=0; ui < GetMenuItemCount(); ui++)
	{
		MENUITEMEX* pItem = new MENUITEMEX();
		pItem->m_bIsTopLevel	= FALSE;
		pItem->m_nListEnabled	= -1;
		pItem->m_nListDisabled	= -1;
		pItem->m_nImageEnabled	= -1;
		pItem->m_nImageDisabled	= -1;
		pItem->m_bHasSubMenu = FALSE;
		pItem->m_bNewSepartor = FALSE;
		GetMenuString(ui, pItem->m_sCaption, MF_BYPOSITION);
		pItem->m_uiID = GetMenuItemID(ui);
		CMenu::ModifyMenu(ui, MF_BYPOSITION|MF_OWNERDRAW, pItem->m_uiID, (TCHAR*) pItem);
		m_TempItems.Add(pItem);
		
		if((pSubMenu = GetSubMenu(ui)) != NULL)
		{  
			pItem->m_bHasSubMenu = TRUE;
			pItem->m_uiID = (UINT)pSubMenu->m_hMenu;
	
			CMenuEx* pMenu = (CMenuEx*)pSubMenu;
			pMenu->MakeOwnerDrawn(bTopLevel);

            //插入分隔符使顶层菜单项高度一致
			MENUITEMEX* pItem = new MENUITEMEX();
			pItem->m_bIsTopLevel	= FALSE;
			pItem->m_nListEnabled	= -1;
			pItem->m_nListDisabled	= -1;
			pItem->m_nImageEnabled	= -1;
			pItem->m_nImageDisabled	= -1;
		    pItem->m_uiID = 0;
			pItem->m_bNewSepartor = TRUE;
			m_TempItems.Add(pItem);

            InsertMenu(ui+1,MF_BYPOSITION|MF_SEPARATOR);
			CMenu::ModifyMenu(ui+1, MF_BYPOSITION|MF_OWNERDRAW, pItem->m_uiID, (TCHAR*) pItem);

			ui++;


		}
	}
}

void CMenuEx::SetItemImages(UINT uiID, INT nImage, CToolBar* pToolBar)
{
	CMenuEx*	pMenu;
	MENUITEMEX* pItem;
	BOOL		bFound;
	INT_PTR		nListEnabled	= -1;
	INT_PTR		nListDisabled	= -1;
	CImageList*	pilEnabled;
	CImageList*	pilDisabled;
	for(INT nMenu=0; nMenu<m_TempMenus.GetSize(); nMenu++)
	{
		pMenu = (CMenuEx*) m_TempMenus[nMenu];
		for(INT nItem=0; nItem<pMenu->m_TempItems.GetSize(); nItem++)
		{
			pItem = (MENUITEMEX*) pMenu->m_TempItems[nItem];
			if(pItem->m_uiID == uiID)
			{
				// Check if ToolBar already included in this menu
				bFound = FALSE;
				for(INT nToolBar=0; nToolBar<pMenu->m_ToolBars.GetSize(); nToolBar++)
				{
					if(pToolBar == (CToolBar*) pMenu->m_ToolBars[nToolBar])
					{
						bFound			= TRUE;
						nListEnabled	= nToolBar;
						nListDisabled	= nToolBar+1;
						break;
					}
				}
				if(!bFound)
				{
					pMenu			->m_ToolBars.Add(pToolBar);
					nListEnabled	= pMenu->m_ImageLists.Add(new CImageList());
					nListDisabled	= pMenu->m_ImageLists.Add(new CImageList());
					pilEnabled		= pToolBar->GetToolBarCtrl().GetImageList();
					pilDisabled		= pToolBar->GetToolBarCtrl().GetDisabledImageList();
					if(pilEnabled )
						((CImageList*) pMenu->m_ImageLists[nListEnabled]) ->Create(pilEnabled );
					else
					{
						delete (CImageList*) pMenu->m_ImageLists[nListEnabled];
						pMenu->m_ImageLists[nListEnabled] = NULL;
					}
					if(pilDisabled)
						((CImageList*) pMenu->m_ImageLists[nListDisabled])->Create(pilDisabled);
					else
					{
						delete (CImageList*) pMenu->m_ImageLists[nListDisabled];
						pMenu->m_ImageLists[nListDisabled] = NULL;
					}
				}

				pItem->m_nListEnabled	= nListEnabled;
				pItem->m_nListDisabled	= nListDisabled;
				pItem->m_nImageEnabled	= nImage;
				pItem->m_nImageDisabled	= nImage;
			}
		}
	}
}

UINT CMenuEx::EnableMenuItem(UINT nIDEnableItem, UINT nEnable)
{
	return	CMenu::EnableMenuItem(nIDEnableItem, nEnable);
	//if( (nEnable&MF_BYPOSITION) == MF_BYPOSITION)
	//{		
	//	return	CMenu::EnableMenuItem(nIDEnableItem, nEnable);
	//}
	//else
	//{  
	//	for(int i = 0; i <m_TempItems.GetSize(); i++)
	//	{
	//		MENUITEMEX* pItem  = (MENUITEMEX*)m_TempItems.GetAt(i);
	//		
	//		if(pItem->m_uiID == nIDEnableItem)
	//		{
	//			UINT nNewEnable = MF_BYPOSITION|(nEnable&(~MF_BYCOMMAND));
 //               return CMenu::EnableMenuItem(i, nNewEnable);
	//			
	//		}
	//	}
	//}
	//
	//return -1;
}
