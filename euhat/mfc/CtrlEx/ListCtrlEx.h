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
	//��д�������������
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, 
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	int InsertColumn( int nCol, const LVCOLUMN* pColumn );
	
	BOOL DeleteColumn(int nCol);
	
	BOOL SetTextColor(COLORREF cr);

	void SetOwnerDrawStyle();
	
public:
	COLORREF m_clrText;//������ɫ
	COLORREF m_clrGrid; //��������ɫ
	COLORREF m_clrHighlight; //ѡ��ʱ����ɫ
	int m_nRowHeight;//�и�
	int m_fontWidth;//������
	int m_fontHeight;//����߶�
	CFont font_;

	CPtrList m_ptrListCol;  //��������ɫ
	CPtrList m_ptrListItem; //����Item��ɫ��
	CPtrList m_colTextColor; //������������ɫ
	CPtrList m_ItemTextColor; //���浥Ԫ��������ɫ

	static HBITMAP  m_hBitmap;
	
	bool FindColColor(int col, COLORREF &color);//��������ɫ
	bool FindColTextColor(int col, COLORREF &color);//������������ɫ
	bool FindItemColor(int col, int row, COLORREF &color);//����Item��ɫ
	bool FindItemTextColor(int col, int row, COLORREF &color);//����Item������ɫ
	
	void SetColColor(int col, COLORREF color);//��������ɫ
	void SetColTextColor(int col, COLORREF color);//�������ı���ɫ
	void SetItemColor(int col, int row, COLORREF color);//����Item��ɫ
	void SetItemTextColor(int col, int row, COLORREF color);//����Item�ı���ɫ
	
	void SetHeaderBKColor(int R, int G, int B, int Gradient=0);//Gradient - ����ϵ�������屳����,���ý�����Ϊ0
	void SetHeaderTextColor(COLORREF color);
	void SetHeaderHeight(int Height);//���ñ�ͷ�߶�
	void SetHeaderFontHW(int nHeight, int nWidth);//���ñ�ͷ�����С
	
	void SetRowHeigt(int nHeight);//�����и�
	void SetFontHW(int nHeight, int nWidth);//��������ĸߺͿ�
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__FA836747_8554_44F9_A150_CF66645D7CFA__INCLUDED_)
