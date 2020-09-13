#pragma once

#include "../MfcBasePage.h"
#include "../Expert.h"

class CfgWorkTraceDlg : public MfcBasePage
{
public:
	CfgWorkTraceDlg(EuhatBase *euhatBase);
	virtual ~CfgWorkTraceDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CFG_WORK_TRACE_DIALOG };
#endif

	int save();

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedKqTimeSpanAdd();
	afx_msg void OnBnClickedKqTimeSpanDel();
	afx_msg void OnDblclkListAttendanceRegulation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickListAttendanceRegulation(NMHDR *pNMHDR, LRESULT *pResult);
	int refreshUiLocalization();

	CListCtrl listAR_;
};
