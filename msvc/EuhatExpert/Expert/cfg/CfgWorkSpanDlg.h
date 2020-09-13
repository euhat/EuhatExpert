#pragma once

#include "../local/WorkTrace/WorkSpan.h"
#include "../MfcBasePage.h"

class CfgWorkSpanDlg : public MfcBasePage
{
public:
	CfgWorkSpanDlg(EuhatBase *euhatBase, CWnd *parent);
	virtual ~CfgWorkSpanDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CFG_WORK_SPAN_DIALOG };
#endif

	WorkSpan v_;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	CDateTimeCtrl dateTimeStartTime_;
	CDateTimeCtrl dateTimeEndTime_;
};
