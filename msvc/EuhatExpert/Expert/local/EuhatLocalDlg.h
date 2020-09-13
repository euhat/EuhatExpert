#pragma once

#include "../MfcBasePage.h"
#include "../../share/EuhatPeerId.h"
#include "../EuhatRightDlg.h"

class EuhatTabCtrl;
class WorkTraceDlg;
class FileServerDlg;

class EuhatLocalDlg : public EuhatRightDlg
{
	void correctPos(CRect &rc);
	void onClick(EuhatTabCtrl &ctrl);

	unique_ptr<WorkTraceDlg> wtDlg_;
	unique_ptr<FileServerDlg> fsDlg_;

public:
	EuhatLocalDlg(EuhatBase *euhatBase);
	virtual ~EuhatLocalDlg();

	void show();
	void hide();

	EuhatPeerId id_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
