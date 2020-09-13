#if !defined(AFX_LISTCTRLEX_H__FA836747_8554_44F9_A150_CF66645D7CFA__INCLUDED_)
#define AFX_LISTCTRLEX_H__FA836747_8554_44F9_A150_CF66645D7CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file
//
#include "HeaderCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

#define  GRID_ROW_PIXEL    1
#define  GRID_COL_PIXEL    2

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();
	
protected:
	DECLARE_MESSAGE_MAP()
		virtual void PreSubclassWindow();
	
public:
	CHeaderCtrlEx m_Header;
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	
private:
	void DeletePtrList(CPtrList &ptrList);
	
public:
	//重写父类的两个函数
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, 
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	int InsertColumn( int nCol, const LVCOLUMN* pColumn );
	
	BOOL DeleteColumn(int nCol);
	
	BOOL SetTextColor(COLORREF cr);

	void SetOwnerDrawStyle();
	
public:
	COLORREF m_clrText;//字体颜色
	COLORREF m_clrGrid; //网格线颜色
	COLORREF m_clrHighlight; //选中时的颜色
	int m_nRowHeight;//行高
	int m_fontWidth;//字体宽度
	int m_fontHeight;//字体高度
	CFont font_;

	CPtrList m_ptrListCol;  //保存列颜色
	CPtrList m_ptrListItem; //保存Item颜色表
	CPtrList m_colTextColor; //保存列字体颜色
	CPtrList m_ItemTextColor; //保存单元格字体颜色

	static HBITMAP  m_hBitmap;
	
	bool FindColColor(int col, COLORREF &color);//查找列颜色
	bool FindColTextColor(int col, COLORREF &color);//查找列字体颜色
	bool FindItemColor(int col, int row, COLORREF &color);//查找Item颜色
	bool FindItemTextColor(int col, int row, COLORREF &color);//查找Item字体颜色
	
	void SetColColor(int col, COLORREF color);//设置列颜色
	void SetColTextColor(int col, COLORREF color);//设置列文本颜色
	void SetItemColor(int col, int row, COLORREF color);//设置Item颜色
	void SetItemTextColor(int col, int row, COLORREF color);//设置Item文本颜色
	
	void SetHeaderBKColor(int R, int G, int B, int Gradient=0);//Gradient - 渐变系数，立体背景用,不用渐变设为0
	void SetHeaderTextColor(COLORREF color);
	void SetHeaderHeight(int Height);//设置表头高度
	void SetHeaderFontHW(int nHeight, int nWidth);//设置表头字体大小
	
	void SetRowHeigt(int nHeight);//设置行高
	void SetFontHW(int nHeight, int nWidth);//设置字体的高和宽
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__FA836747_8554_44F9_A150_CF66645D7CFA__INCLUDED_)
