#pragma once

#include "MfcBasePage.h"
#include "../share/EuhatPeerId.h"
#include <EuhatTabCtrl.h>

class EuhatLeftDlg;

class EuhatRightDlg : public MfcBasePage, public EuhatTabCtrl::Listener
{
	void onClick(EuhatTabCtrl &ctrl);

protected:
	unique_ptr<EuhatTabCtrl> euTabs_;

public:
	EuhatRightDlg(int idd, EuhatBase *euhatBase);
	virtual ~EuhatRightDlg();

	virtual void correctPos(CRect &rc);
	virtual void show();
	virtual void hide();

	EuhatPeerId id_;
	EuhatLeftDlg *leftDlg_;
	int isVisible_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
};
