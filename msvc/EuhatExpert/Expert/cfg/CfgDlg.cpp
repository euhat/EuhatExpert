#include "stdafx.h"
#include <common/OpCommon.h>
#include "../Expert.h"
#include "../ExpertDlg.h"
#include "CfgDlg.h"
#include "CfgCommonDlg.h"
#include "CfgWorkTraceDlg.h"
#include "afxdialogex.h"
#include <EuhatPostDefMfc.h>

//IMPLEMENT_DYNAMIC(CfgDlg, CDialogEx)

CfgDlg::CfgDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_CFG_DIALOG, euhatBase)
{

}

CfgDlg::~CfgDlg()
{
}

void CfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CFG_ITEM, listBoxCfg_);
}

BEGIN_MESSAGE_MAP(CfgDlg, CDialogEx)
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_LIST_CFG_ITEM, &CfgDlg::OnSelchangeListCfgItem)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

int CfgDlg::addPage(const char *name, MfcBasePage *page)
{
	CString cstr = CA2T(name);
	listBoxCfg_.insertStr(listBoxCfg_.GetCount(), cstr);

	page->Create(page->idd_, this);
	CRect rc;
	GetDlgItem(IDC_STATIC_CFG_FRAME)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	page->MoveWindow(&rc);
	page->BringWindowToTop();
	page->ShowWindow(SW_SHOW);

	pages_.push_back(move(unique_ptr<MfcBasePage>(page)));

	return 1;
}

BOOL CfgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	pushUiLoc("GENERAL", "General");
	pushUiLoc("WORK_TRACE", "Work Trace");

	addPage("General", new CfgCommonDlg(euhatBase_));
	addPage("Work Trace", new CfgWorkTraceDlg(euhatBase_));

	listBoxCfg_.SetCurSel(0);
	OnSelchangeListCfgItem();

	return TRUE;
}

int CfgDlg::refreshUiLocalization()
{
	listBoxCfg_.ResetContent();

	listBoxCfg_.insertStr(listBoxCfg_.GetCount(), t("GENERAL"));
	listBoxCfg_.insertStr(listBoxCfg_.GetCount(), t("WORK_TRACE"));

	listBoxCfg_.SetCurSel(0);

	return 1;
}

void CfgDlg::OnPaint()
{
	CPaintDC dc(this);
}

void CfgDlg::OnSelchangeListCfgItem()
{
	int curIdx = listBoxCfg_.GetCurSel();
	if (curIdx < 0)
		return;

	int i = 0;
	for (vector<unique_ptr<MfcBasePage>>::iterator it = pages_.begin(); it != pages_.end(); it++, i++) {
		if (curIdx == i) {
			(*it)->ShowWindow(SW_SHOW);
		}
		else {
			(*it)->ShowWindow(SW_HIDE);
		}
	}
}

void CfgDlg::OnClose()
{
	ShowWindow(SW_HIDE);

//	MfcBasePage::OnClose();
}
