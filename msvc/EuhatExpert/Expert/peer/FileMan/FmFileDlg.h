#pragma once

#include "../../MfcBasePage.h"
#include <EuhatListCtrl.h>
#include <EuhatIcon.h>
#include <os/EuhatPath.h>
#include <common/JyDataStream.h>
#include <EuhatTabCtrl.h>

class FileManSubDlg;
class JyMsgLoop;

class FmFileDlg : public MfcBasePage, public EuhatListCtrl::User, public EuhatTabCtrl::Listener
{
	void onListCtrlDbClick(EuhatListCtrl *listCtrl, WPARAM wParam, int x, int y);
	void onClick(EuhatTabCtrl &ctrl);

public:
	FmFileDlg(EuhatBase *euhatBase);
	virtual ~FmFileDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_MAN_FILE_DIALOG};
#endif

	void correctPos();
	void correctPos(CRect &rc, int margin, EuhatListCtrl *ctrl);
	void displayFiles(JyDataReadStream &ds);

	unique_ptr<EuhatIconSysPool> iconPool_;
	unique_ptr<EuhatListCtrl> listCtrl_;
	unique_ptr<EuhatTabCtrl> euTabs_;
	
	JyMsgLoop *engine_;

	EuhatPath curDir_;

	FileManSubDlg *parentDlg_;

	int isLocal_;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnGoUpFolder();
public:
	afx_msg void OnBnClickedBtnEnterDir();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnChangeEditCurDir();
	afx_msg void OnMaxtextEditCurDir();
};
