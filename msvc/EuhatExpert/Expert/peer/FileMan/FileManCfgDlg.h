#pragma once

#include "../../MfcBasePage.h"

class FileManDlg;
class EuhatCertificateDlg;

class FileManCfgDlg : public MfcBasePage
{
	void correctPos();

public:
	FileManCfgDlg(EuhatBase *euhatBase);
	virtual ~FileManCfgDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_MAN_CFG_DIALOG };
#endif

	void loadData();
	void saveData(const char *peerName);
	void dataToUi();
	int uiToData();

	string peerName_;
	string ip_;
	int port_;
	string visitCode_;

	FileManDlg *parent_;
	unique_ptr<EuhatCertificateDlg> certDlg_;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnGo();
	afx_msg void OnBnClickedBtnCancel();
};
