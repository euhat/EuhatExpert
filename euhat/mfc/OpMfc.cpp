#include "stdafx.h"
#include "OpMfc.h"
#include <shlobj.h>
#include <EuhatPostDefMfc.h>

LRESULT whEditCtrlAddMsg(CEdit &editOutput, WPARAM wParam, LPARAM lParam)
{
	char *msg = (char *)lParam;

	time_t t = time(NULL);
	tm *local = localtime(&t);
	char timeBuf[256];
	strftime(timeBuf, 254, "[%Y-%m-%d %H:%M:%S] ", local);
	string dispStr = timeBuf;
	dispStr += msg;
	free(msg);

	CString cline = CA2T(dispStr.c_str());
	CString cstr;
	editOutput.GetWindowText(cstr);
	if (cstr.GetLength() > 3000)
	{
		cstr = cstr.Right(cstr.GetLength() - cstr.Find(_T("\n")) - 1);
	}
	cstr += cline;
	editOutput.SetWindowText(cstr);
	editOutput.SetModify(FALSE);

	int length = editOutput.GetWindowTextLength();
	editOutput.SetSel(length, length);

	return TRUE;
}

void whEditCtrlMsg(HWND hwnd, const char *txt)
{
	::PostMessage(hwnd, WH_USER_MSG_ADD_MSG, 0, (LPARAM)_strdup(txt));
}

int whBrowsePath(HWND hwnd, CString &cstr)
{
	BROWSEINFO bi;
	LPITEMIDLIST pidl;
	bi.hwndOwner = hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = _T("");
	bi.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;

	if ((pidl = SHBrowseForFolder(&bi)) != NULL)
	{
		TCHAR path[MAX_PATH] = { 0 };
		SHGetPathFromIDList(pidl, path);
		cstr = path;
		return 1;
	}
	return 0;
}