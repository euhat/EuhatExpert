#if !defined(AFX_NEWCOMBOBOX_H__BC428B4F_90EB_4A13_9499_6D3F818283A9__INCLUDED_)
#define AFX_NEWCOMBOBOX_H__BC428B4F_90EB_4A13_9499_6D3F818283A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewComboBox window

class CNewComboBox : public CComboBox
{
// Construction
public:
	CNewComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bDown;
	HBITMAP	m_hbmpDownBtn;	//下拉按钮的图片

	virtual ~CNewComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewComboBox)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWCOMBOBOX_H__BC428B4F_90EB_4A13_9499_6D3F818283A9__INCLUDED_)
