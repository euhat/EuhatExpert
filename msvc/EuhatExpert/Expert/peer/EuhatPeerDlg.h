#pragma once

#include "../MfcBasePage.h"
#include "../../share/EuhatPeerId.h"
#include "../EuhatRightDlg.h"

class EuhatTabCtrl;
class FileManDlg;

class EuhatPeerDlg : public EuhatRightDlg
{
	void onClick(EuhatTabCtrl &ctrl);

	unique_ptr<FileManDlg> fmDlg_;

public:
	EuhatPeerDlg(EuhatBase *euhatBase);
	virtual ~EuhatPeerDlg();

	void correctPos(CRect &rc);
	void show();
	void hide();

	EuhatPeerId id_;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
