#if !defined(AFX_NEWDATETIMECTRL_H__BFFBA54F_31C3_458C_8F23_0D8F3AAE19C5__INCLUDED_)
#define AFX_NEWDATETIMECTRL_H__BFFBA54F_31C3_458C_8F23_0D8F3AAE19C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewDateTimeCtrl.h : header file
//
#include "NewSpinButton.h"

/////////////////////////////////////////////////////////////////////////////
// CNewDateTimeCtrl window

class CNewDateTimeCtrl : public CDateTimeCtrl
{
// Construction
public:
	CNewDateTimeCtrl();

// Attributes
public:

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewDateTimeCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CNewSpinButton m_SpinBtn;
//	CSpinButtonCtrl* GetSpinBtnCtrl();
	virtual ~CNewDateTimeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNewDateTimeCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWDATETIMECTRL_H__BFFBA54F_31C3_458C_8F23_0D8F3AAE19C5__INCLUDED_)
