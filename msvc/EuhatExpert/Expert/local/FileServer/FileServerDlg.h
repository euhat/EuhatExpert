#pragma once

#include <mfc/OpMfc.h>
#include "../../MfcBasePage.h"

class FileManServer;
class EuhatCertificateDlg;

class FileServerDlg : public MfcBasePage
{
	void correctPos();

	unique_ptr<FileManServer> srv_;
	unique_ptr<EuhatCertificateDlg> certDlg_;
	CEdit editOutput_;
	CEdit editVisitCode_;

public:
	FileServerDlg(EuhatBase *euhatBase);
	virtual ~FileServerDlg();

	void msg(const char *txt);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_SERVER_DIALOG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnBrowseDir();
	LRESULT onAddMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnShowHide();
};
