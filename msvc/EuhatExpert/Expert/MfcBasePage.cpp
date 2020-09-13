#include "stdafx.h"
#include "base/EuhatBase.h"
#include "MfcBasePage.h"
#include <EuhatPostDefMfc.h>

MfcBasePage::MfcBasePage(int idd, EuhatBase *euhatBase, CWnd *parent)
	: CDialogEx(idd, parent == NULL? (CWnd *)euhatBase->mainWnd_ : parent)
{
	idd_ = idd;
	euhatBase_ = euhatBase;
}

MfcBasePage::~MfcBasePage()
{

}

void MfcBasePage::create(CWnd *parent)
{
	Create((UINT)idd_, parent == NULL? (CWnd *)euhatBase_->mainWnd_ : parent);
}

void MfcBasePage::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
}

BOOL MfcBasePage::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	else return CDialogEx::PreTranslateMessage(pMsg);
}

void MfcBasePage::pushUiLoc(const char *id, const char *name)
{
	locText_.push_back(pair<string, string>(id, name));
}

CString MfcBasePage::t(const char *id)
{
	for (vector<pair<string, string> >::iterator it = locText_.begin(); it != locText_.end(); it++)
	{
		if (it->first == id)
		{
			CString str = CA2T(it->second.c_str(), CP_UTF8);
			return str;
		}
	}
	return _T("");
}

int MfcBasePage::refreshUiLocalization()
{
	return 1;
}