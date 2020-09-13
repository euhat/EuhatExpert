#pragma once

#include <common/OpCommon.h>

class EuhatBase;

class MfcBasePage : public CDialogEx
{

protected:
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	void pushUiLoc(const char *id, const char *name);
	CString t(const char *id);

	EuhatBase *euhatBase_;

public:
	MfcBasePage(int idd, EuhatBase *euhatBase, CWnd *parent = NULL);
	virtual ~MfcBasePage();

	virtual int refreshUiLocalization();
	void create(CWnd *parent = NULL);

	int idd_;
	int isNew_;
	vector<pair<string, string> > locText_;
};