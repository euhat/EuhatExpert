#if !defined(AFX_DATETIME_H__FD0FFE40_0006_45E8_A0A2_E1CB780AF4B9__INCLUDED_)
#define AFX_DATETIME_H__FD0FFE40_0006_45E8_A0A2_E1CB780AF4B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DateTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDateTime window

class CNewDateCtrl : public CDateTimeCtrl
{
// Construction
public:
	CNewDateCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDateTime)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNewDateCtrl();
	BOOL m_bDown;
	HBITMAP	m_hbmpDownBtn;	//下拉按钮的图片


	// Generated message map functions
protected:
	//{{AFX_MSG(CDateTime)
	afx_msg void OnPaint();
	afx_msg void OnDropdown(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnCloseup(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATETIME_H__FD0FFE40_0006_45E8_A0A2_E1CB780AF4B9__INCLUDED_)
