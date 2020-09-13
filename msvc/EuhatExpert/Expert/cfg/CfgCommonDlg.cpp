#include "stdafx.h"
#include <common/OpCommon.h>
#include "../Expert.h"
#include "../ExpertDlg.h"
#include "CfgDlg.h"
#include "CfgCommonDlg.h"
#include "afxdialogex.h"
#include <EuhatPostDefMfc.h>

CfgCommonDlg::CfgCommonDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_CFG_COMMON_DIALOG, euhatBase)
{

}

CfgCommonDlg::~CfgCommonDlg()
{
}

void CfgCommonDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SELECT_LANGUAGE, comboSelectLanguage_);
}

BEGIN_MESSAGE_MAP(CfgCommonDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_AUTO_START_WHEN_BOOTING, &CfgCommonDlg::OnClickedCheckAutoStartWhenBooting)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_LANGUAGE, &CfgCommonDlg::OnSelchangeComboSelectLanguage)
	ON_BN_CLICKED(IDC_BTN_POWER_OFF_SCREEN, &CfgCommonDlg::OnBnClickedBtnPowerOffScreen)
END_MESSAGE_MAP()

#include <Shlobj.h>
#pragma comment(lib, "shell32")

BOOL createLink(const TCHAR *szPath, const TCHAR *szLink)
{
	HRESULT hres;
	IShellLink *psl;
	IPersistFile *ppf;
 
	CoInitialize(NULL);
  
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **)&psl);
	if (FAILED(hres)) {
		CoUninitialize();
		return FALSE;
	}
 
	psl->SetPath(szPath);
//	psl->SetHotkey(MAKEWORD('R', HOTKEYF_SHIFT|HOTKEYF_CONTROL));  

	CString csWorkDir(szPath);
	int idx = csWorkDir.ReverseFind('\\');
	csWorkDir = csWorkDir.Left(idx);
	psl->SetWorkingDirectory(csWorkDir);

	hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hres)) {
		CoUninitialize();
		return FALSE;
	}

	hres = ppf->Save(szLink, STGM_READWRITE);
  
	ppf->Release();
	psl->Release();

	CoUninitialize();

	return TRUE;
}

BOOL deleteLink(const TCHAR *lpszShortcut)
{
	SHFILEOPSTRUCT fos;
	ZeroMemory(&fos, sizeof(fos));
	fos.hwnd = HWND_DESKTOP;
	fos.wFunc = FO_DELETE;
	fos.pFrom = lpszShortcut;
	fos.pTo = NULL;
	fos.fFlags = FOF_SILENT | FOF_ALLOWUNDO;
	int ret = SHFileOperation(&fos);
	if (0 != ret)
		return FALSE;
	return TRUE;
}

void CfgCommonDlg::OnClickedCheckAutoStartWhenBooting()
{
#if 0
	HKEY hKey;
	LPCTSTR regRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, regRun, 0, KEY_WRITE, &hKey);
	if (lRet != ERROR_SUCCESS) {
		CString msg;
		CString formatStr;
		formatStr.LoadString(IDS_MSG_OPEN_REG_KEY_FAILED);
		msg.Format(formatStr, regRun);
		MessageBox(msg);
		return;
	}

	if (IsDlgButtonChecked(IDC_CHECK_AUTO_START_WHEN_BOOTING)) {
		TCHAR filePath[MAX_PATH] = { 0 };
		DWORD dwRet = GetModuleFileName(NULL, filePath, MAX_PATH - 1);
		CString cmd;
		cmd.Format(_T("\"%s\" -hide"), filePath);
		lRet = RegSetValueEx(hKey, _T("EuhatExpert"), 0, REG_SZ, (BYTE *)(LPCTSTR)cmd, cmd.GetLength());
	} else {
		lRet = RegDeleteValue(hKey, _T("EuhatExpert"));
	}

	RegCloseKey(hKey);

#else
	char szFilePath[MAX_PATH] = { 0 };
	GetEnvironmentVariableA("UserProfile", szFilePath, sizeof(szFilePath) - 1);
	strcat(szFilePath, "\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\");
	strcat(szFilePath, "Euhat Expert.lnk");

	CString linkPath = CA2T(szFilePath);

	if (IsDlgButtonChecked(IDC_CHECK_AUTO_START_WHEN_BOOTING)) {
		TCHAR exePath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, exePath, MAX_PATH - 1);
		createLink(exePath, (LPCTSTR)linkPath);
	} else {
		DeleteFile((LPCTSTR)linkPath);
	}
	long lRet = ERROR_SUCCESS;
#endif

	if (lRet != ERROR_SUCCESS) {
		CString errStr;
		errStr.LoadString(IDS_MSG_WRITE_REG_FAILED_FOR_AUTO_STARTING_WHEN_BOOTING);
		AfxMessageBox(errStr);
	} else {
		save();
	}
}

BOOL CfgCommonDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	if (euhatBase_->isAutoRunWhenReboot_ != 0) {
		((CButton *)GetDlgItem(IDC_CHECK_AUTO_START_WHEN_BOOTING))->SetCheck(TRUE);
	}

	OnClickedCheckAutoStartWhenBooting();

	comboSelectLanguage_.InsertString(comboSelectLanguage_.GetCount(), _T("English"));
	comboSelectLanguage_.InsertString(comboSelectLanguage_.GetCount(), _T("中文"));
	comboSelectLanguage_.SetCurSel(euhatBase_->curUiLanguage_);

	return TRUE;
}

int CfgCommonDlg::refreshUiLocalization()
{
	comboSelectLanguage_.SetCurSel(euhatBase_->curUiLanguage_);
	return 1;
}

int CfgCommonDlg::save()
{
	euhatBase_->isAutoRunWhenReboot_ = IsDlgButtonChecked(IDC_CHECK_AUTO_START_WHEN_BOOTING);
	euhatBase_->curUiLanguage_ = comboSelectLanguage_.GetCurSel();

	euhatBase_->save();

	return 1;
}

void CfgCommonDlg::OnSelchangeComboSelectLanguage()
{
	int selIdx = comboSelectLanguage_.GetCurSel();
	if (selIdx < 0)
		return;

	if (selIdx != euhatBase_->curUiLanguage_)
	{
		save();
		euhatBase_->refreshUiLocalization();
	}
}

void CfgCommonDlg::OnBnClickedBtnPowerOffScreen()
{
	HWND hWnd = ::FindWindow(0, 0);
	::SendMessage(hWnd, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
}
