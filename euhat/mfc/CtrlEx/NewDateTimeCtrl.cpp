// NewDateTimeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "NewDateTimeCtrl.h"
#include "NewSpinButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewDateTimeCtrl

CNewDateTimeCtrl::CNewDateTimeCtrl()
{

}

CNewDateTimeCtrl::~CNewDateTimeCtrl()
{
}


BEGIN_MESSAGE_MAP(CNewDateTimeCtrl, CDateTimeCtrl)
	//{{AFX_MSG_MAP(CNewDateTimeCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewDateTimeCtrl message handlers
BOOL CALLBACK ChildWindowCallback(HWND hChild, LPARAM lParam)
{
	CNewDateTimeCtrl *pdateTime = (CNewDateTimeCtrl*)lParam;
	TCHAR szName[MAX_PATH] = {0};
	GetClassName(hChild, szName, sizeof(szName));
	if(pdateTime->m_SpinBtn.GetSafeHwnd() == NULL && _tcscmp(szName, _T("msctls_updown32")) == 0)
	{
	
		//HWND hParent = GetParent(hChild);		
		//GetParent(hParent);
		pdateTime->m_SpinBtn.SubclassWindow(hChild);
			
	}
	
	return TRUE;
}

void CNewDateTimeCtrl::OnPaint() 
{
//	CPaintDC dc(this); // device context for painting
	CDateTimeCtrl::OnPaint();
	
	
	// Do not call CDateTimeCtrl::OnPaint() for painting messages
}

void CNewDateTimeCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDateTimeCtrl::PreSubclassWindow();
	EnumChildWindows(this->GetSafeHwnd(),ChildWindowCallback, (LPARAM)this);
}

