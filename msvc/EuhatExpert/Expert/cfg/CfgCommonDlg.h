#pragma once

#include "../MfcBasePage.h"

class CfgCommonDlg : public MfcBasePage
{
	int save();

public:
	CfgCommonDlg(EuhatBase *euhatBase);
	virtual ~CfgCommonDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CFG_COMMON_DIALOG };
#endif

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange *pDX);
	afx_msg void OnClickedCheckAutoStartWhenBooting();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboSelectLanguage();
	int refreshUiLocalization();

	CComboBox comboSelectLanguage_;
public:
	afx_msg void OnBnClickedBtnPowerOffScreen();
};
