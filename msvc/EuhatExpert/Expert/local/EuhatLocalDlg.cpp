#include "stdafx.h"
#include "../Expert.h"
#include "../ExpertDlg.h"
#include "EuhatLocalDlg.h"
#include "WorkTrace/WorkTraceDlg.h"
#include "FileServer/FileServerDlg.h"
#include "afxdialogex.h"
#include <EuhatTabCtrl.h>
#include <EuhatPostDefMfc.h>

EuhatLocalDlg::EuhatLocalDlg(EuhatBase *euhatBase)
	: EuhatRightDlg(IDD_EMPTY_CHILD_DIALOG, euhatBase)
{

}

EuhatLocalDlg::~EuhatLocalDlg()
{

}

void EuhatLocalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(EuhatLocalDlg, CDialogEx)
END_MESSAGE_MAP()

void EuhatLocalDlg::correctPos(CRect &rc)
{
	if (NULL == wtDlg_.get())
		return;

	EuhatRightDlg::correctPos(rc);

	wtDlg_->MoveWindow(rc);
	fsDlg_->MoveWindow(rc);
}

BOOL EuhatLocalDlg::OnInitDialog()
{
	EuhatRightDlg::OnInitDialog();

	wtDlg_.reset(new WorkTraceDlg(euhatBase_));
	wtDlg_->create();

	fsDlg_.reset(new FileServerDlg(euhatBase_));
	fsDlg_->create();

	euTabs_->add("WorkTrace");
	euTabs_->add("FileServer");
	euTabs_->setCurSel(0);

	return TRUE;
}

void EuhatLocalDlg::show()
{
	wtDlg_->ShowWindow(SW_HIDE);
	fsDlg_->ShowWindow(SW_HIDE);

	int curSel = euTabs_->getCurSel();
	if (curSel == 0)
	{
		wtDlg_->BringWindowToTop();
		wtDlg_->ShowWindow(SW_SHOW);
	}
	else if (curSel == 1)
	{
		fsDlg_->BringWindowToTop();
		fsDlg_->ShowWindow(SW_SHOW);
	}

	EuhatRightDlg::show();
}

void EuhatLocalDlg::hide()
{
	wtDlg_->ShowWindow(SW_HIDE);
	fsDlg_->ShowWindow(SW_HIDE);

	EuhatRightDlg::hide();
}

void EuhatLocalDlg::onClick(EuhatTabCtrl &ctrl)
{
	show();
}