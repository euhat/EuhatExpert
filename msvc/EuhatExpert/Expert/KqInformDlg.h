#pragma once
#include <common/OpCommon.h>
#include "MfcBasePage.h"
#include <common/WhCommon.h>

class KqInformDlg : public MfcBasePage
{
	int isOpened_;
	int hasInChecked_;
	int hasOutChecked_;
	int isInOrOut_;
	time_t dispTime_;

public:
	KqInformDlg(EuhatBase *euhatBase);
	virtual ~KqInformDlg();

	int open();
	int close();
	int check();

	int displayOpProc();

	enum { IDD = IDD_KQ_INFORM_DIALOG };

	WhThreadHandle displayOpThreadHandle_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	int refreshUiLocalization();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
public:
	afx_msg void OnDestroy();
};
