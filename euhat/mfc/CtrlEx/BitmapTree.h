#if !defined(AFX_BITMAPTREE_H__F5096A94_2828_423F_95B1_118CE4B2106A__INCLUDED_)
#define AFX_BITMAPTREE_H__F5096A94_2828_423F_95B1_118CE4B2106A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapTree.h : header file
//



class CBitmapTree : public CTreeCtrl
{
	// Construction
public:
	CBitmapTree();
	HBITMAP  m_hBitmap;
   
	// Attributes
public:

	CBrush m_BackBrush;	
	CBrush m_GrayBrush;	
	CBrush m_BackBrushNormal;

	COLORREF m_clrText;
	COLORREF m_clrTextSelected;
	COLORREF m_clrMask;

   	int m_iBitmapWidth;
	int m_iBitmapHeight;



	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapTree)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	void  SetBitmap(HBITMAP hbmp);
	virtual ~CBitmapTree();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapTree)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void  OnCustomDraw(LPNMHDR pNmhdr, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) ;

	//}}AFX_MSG
	
	void DrawTreeItem(int nItemState, CRect rcItem, HDC hdc, HTREEITEM hItem);

	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPTREE_H__F5096A94_2828_423F_95B1_118CE4B2106A__INCLUDED_)
