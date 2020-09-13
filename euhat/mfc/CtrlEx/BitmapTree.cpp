// BitmapTree.cpp : implementation file
//
#include "stdafx.h"
#include "BitmapTree.h"

#pragma  comment(lib,"msimg32.lib")

/////////////////////////////////////////////////////////////////////////////
// CBitmapTree

CBitmapTree::CBitmapTree()
{
	m_hBitmap = NULL;

	m_BackBrush.CreateSolidBrush(RGB(254,255,205)); //49,106,197	
	m_GrayBrush.CreateSolidBrush(RGB(236,233,216));//	 
	m_BackBrushNormal.CreateSolidBrush(RGB(45,45,45));

	m_clrText = RGB(255,255,255);
	m_clrTextSelected = RGB(0,0,0);
	m_clrMask = RGB(45,45,45);

	m_iBitmapWidth = 16;
	m_iBitmapHeight = 16;
}

CBitmapTree::~CBitmapTree()
{
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
	}
	m_BackBrush.DeleteObject();
	m_GrayBrush.DeleteObject();
	m_BackBrushNormal.DeleteObject();

}

BEGIN_MESSAGE_MAP(CBitmapTree, CTreeCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw) 
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapTree message handlers


BOOL CBitmapTree::OnEraseBkgnd(CDC* pDC)  
{ 
	if(m_hBitmap ==NULL)
	{
		return CTreeCtrl::OnEraseBkgnd(pDC);
	}

	CDC dcMem; 
	dcMem.CreateCompatibleDC(pDC); 

	HBITMAP pOldBitmap = (HBITMAP)dcMem.SelectObject(m_hBitmap); 

	CRect rc;
	GetClientRect(&rc);
	for(int i = 0; i < rc.Height()/m_iBitmapHeight+1; i++)
	{
		pDC->StretchBlt(0,i*m_iBitmapHeight,rc.Width(),m_iBitmapHeight,&dcMem,0,1,m_iBitmapWidth,m_iBitmapHeight-2,SRCCOPY); //
	}

	dcMem.SelectObject(pOldBitmap); 

	return TRUE; 
} 

void CBitmapTree::OnCustomDraw(LPNMHDR pNmhdr, LRESULT* pResult)
{
	if(m_hBitmap == NULL)
	{
		*pResult = CDRF_DODEFAULT;
		return ;
	}

	static CRect	rcItem;
	static int		nItemState;

	LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)pNmhdr;

	switch (pCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// 这种情况必须处理，且必须将pResult设置为 CDRF_NOTIFYITEMDRAW, 
		// 否则父窗口怎么也收不到 CDDS_ITEMPREPAINT 通知消息 (GGH) 
		*pResult = CDRF_NOTIFYITEMDRAW;

		// 重新定位视图窗口，这样 TreeCtrl 的 DefWindowProc 不会重画
		//::SetViewportOrgEx(pCustomDraw->nmcd.hdc, /*m_nOffset*/0, 0, NULL);
		break;
	case CDDS_ITEMPREPAINT:
		// 设置背景和前景颜色
		nItemState = pCustomDraw->nmcd.uItemState;
		pCustomDraw->nmcd.uItemState &= ~CDIS_FOCUS;
		pCustomDraw->clrText = RGB(255,255,255);

		// 记住绘制项目的矩形
		GetItemRect((HTREEITEM) pCustomDraw->nmcd.dwItemSpec, &rcItem, TRUE);
		*pResult = CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPOSTPAINT:
		DrawTreeItem(nItemState, rcItem, pCustomDraw->nmcd.hdc,(HTREEITEM) pCustomDraw->nmcd.dwItemSpec);
		break;
	default:
		*pResult = CDRF_DODEFAULT;
	}
} 

void CBitmapTree::DrawTreeItem(int nItemState, CRect rcItem, HDC hdc, HTREEITEM hItem)
{
	// 如果此项获得输入焦点，则绘制外围矩形并用蓝色填充矩形区域

	if(rcItem.Width() <= 0 || rcItem.Height() <= 0 || m_hBitmap == NULL)
	{
		return ;
	}

	COLORREF clrText = m_clrText;

	//m_iBitmapHeight应大于rcItem.Height();

	//处理ICON图片。
	int iIconWidth  = 20; //原始图片为16*16,(这里加上2像素的边界)

	CRect rc;
	GetClientRect(&rc);	

	HBITMAP hbmp = CreateCompatibleBitmap(hdc,rc.Width(),m_iBitmapHeight);
	HDC dcMem = CreateCompatibleDC(hdc); 
	HBITMAP pOldBitmap = (HBITMAP)::SelectObject(dcMem,hbmp);

	HDC dcMem2 = CreateCompatibleDC(hdc); 
	HBITMAP pOldBitmap2 = (HBITMAP)::SelectObject(dcMem2,m_hBitmap);


	::StretchBlt(dcMem,0,0,rc.Width(),m_iBitmapHeight,dcMem2,0,1,m_iBitmapWidth,m_iBitmapHeight-2,SRCCOPY); 

	::TransparentBlt(dcMem,rcItem.left - iIconWidth,0,iIconWidth,rcItem.Height(),hdc,rcItem.left - iIconWidth,rcItem.top,iIconWidth,rcItem.Height(),m_clrMask);
	::StretchBlt(hdc,rcItem.left -iIconWidth ,rcItem.top,iIconWidth,rcItem.Height(),dcMem,rcItem.left-iIconWidth,0,iIconWidth, rcItem.Height(),SRCCOPY);

	//处理选中文字区域	
	if(nItemState & CDIS_FOCUS)
	{
		::FillRect(hdc, &rcItem, (HBRUSH)m_BackBrush.m_hObject);
		// 新的焦点矩形代码......
		//::DrawFocusRect( hdc, &rcItem);
		clrText = m_clrTextSelected;
	}
	else if(nItemState & CDIS_SELECTED)
	{
		//没进入此if
		::FillRect(hdc, &rcItem, (HBRUSH)m_BackBrush.m_hObject/*m_GrayBrush.m_hObject*/);
		clrText = m_clrTextSelected;
	}
	else
	{
		// 清除剩余的高亮条
		if(GetSelectedItem() != hItem)
		{		
			::BitBlt(hdc,rcItem.left,rcItem.top,rcItem.Width(),rcItem.Height(),dcMem,rcItem.left,0,SRCCOPY);			
		}
		else
		{
			///			
		}

	}

	//输出文字。
	::SetBkMode(hdc, TRANSPARENT);
	::SetTextColor(hdc,clrText);
	CString str = GetItemText(hItem);
	::DrawText(hdc, str, -1, &rcItem, DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);

	//清理
	::SelectObject(dcMem2,pOldBitmap2);
	DeleteDC(dcMem2);
	::SelectObject(dcMem,pOldBitmap);
	DeleteDC(dcMem);
	DeleteObject(hbmp);
}      

void CBitmapTree::SetBitmap(HBITMAP hbmp)
{
	if(hbmp == NULL)
	{
		return;
	}
	m_hBitmap = hbmp;

	BITMAP bm; 
	GetObject(m_hBitmap,sizeof(BITMAP),(PSTR)&bm); 
	m_iBitmapWidth = bm.bmWidth;
	m_iBitmapHeight = bm.bmHeight;
}

void CBitmapTree::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(m_hBitmap != NULL)
	{
		Invalidate(FALSE);
	}		
	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CBitmapTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(m_hBitmap != NULL)
	{
		Invalidate(FALSE);
	}

	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
BOOL CBitmapTree::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(m_hBitmap != NULL)
	{
		Invalidate(FALSE);

	}	return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

void CBitmapTree::OnItemExpanding(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	if( m_hBitmap != NULL ) 
	{
		Invalidate(FALSE);
	}
	*pResult = 0;
}