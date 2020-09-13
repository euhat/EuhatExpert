#pragma once

#include "../../MfcBasePage.h"

class EuhatCertificateDlg : public MfcBasePage
{
	void correctPos();

public:
	EuhatCertificateDlg(EuhatBase *euhatBase);
	virtual ~EuhatCertificateDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CERTIFICATE_DIALOG };
#endif

	void updateResultMsg();
	int isGenerated();

	CComboBox comboCertLen_;
	string certName_;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnGo();

	afx_msg void OnBnClickedBtnCancel();
	afx_msg void OnBnClickedBtnGenerate();
public:
	afx_msg void OnSelchangeComboCertLength();
};
