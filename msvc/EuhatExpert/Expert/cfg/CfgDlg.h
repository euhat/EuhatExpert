#pragma once

#include <common/OpCommon.h>
#include "../MfcBasePage.h"
#include "../Expert.h"
#include <mfc/JyListBox.h>

class CfgDlg : public MfcBasePage
{
//	DECLARE_DYNAMIC(CfgDlg)

public:
	CfgDlg(EuhatBase *euhatBase);
	virtual ~CfgDlg();

	int addPage(const char *name, MfcBasePage *page);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CFG_DIALOG };
#endif

	vector<unique_ptr<MfcBasePage>> pages_;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeListCfgItem();
	afx_msg void OnClose();
	int refreshUiLocalization();

	JyListBox listBoxCfg_;
};
