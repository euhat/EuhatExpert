#pragma once

#include "../../MfcBasePage.h"
#include <app/FileMan/client/FileManClient.h>
#include <EuhatScrollHBar.h>
#include <EuhatListCtrl.h>

class FileManCfgDlg;
class EuhatPeerDlg;
class FileManSubDlg;

class FileManDlg : public MfcBasePage, public EuhatScrollBar::User, public EuhatListCtrl::User
{
	void correctPos();
	void onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset);

	unique_ptr<FileManSubDlg> subDlg_;
	unique_ptr<EuhatScrollHBar> scrollX_;

	CRect subRc_;

public:
	FileManDlg(EuhatBase *euhatBase);
	virtual ~FileManDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_MAN_DIALOG };
#endif

	int show();
	void resetFileManClient();
	void dataToEngine();
	void connect();

	EuhatPeerDlg *parentDlg_;
	unique_ptr<FileManCfgDlg> cfgDlg_;
	unique_ptr<EuhatListCtrl> listTask_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg void OnBnClickedBtnConnect();
};
