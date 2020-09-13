#pragma once
#include <common/OpCommon.h>
#include "MfcBasePage.h"
#include <mfc/JyListBox.h>
#include <EuhatTabCtrl.h>

class DbOpSqlite;
class DbOpSqliteTable;
class EuhatRightDlg;

class EuhatLeftItemTable
{
public:
	EuhatLeftItemTable(DbOpSqlite *db);

	unique_ptr<DbOpSqliteTable> tab_;
};

class EuhatLeftDlg : public MfcBasePage, public EuhatTabCtrl::Listener
{
	void onClick(EuhatTabCtrl &ctrl);

	unique_ptr<EuhatTabCtrl> euTabs_;
	JyListBox peers_;
	CRect rcPeer_;
	EuhatLeftItemTable tab_;
	string unnameStr_;

	friend class FileManDlg;

public:
	EuhatLeftDlg(EuhatBase *euhatBase);
	virtual ~EuhatLeftDlg();
	void correctPos(CRect &rc);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LEFT_DIALOG };
#endif

	void modifyItem(EuhatRightDlg *obj, const char *newName);
	void addItem(const char *name = "");
	void delItem(int idx);
	int getItem(EuhatRightDlg *obj);
	int getItem(const char *name);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListPeers();

	afx_msg void OnDestroy();
	afx_msg void OnPaint();
};
