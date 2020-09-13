#include "stdafx.h"
#include "../Expert.h"
#include "../ExpertDlg.h"
#include "EuhatPeerDlg.h"
#include "FileMan/FileManDlg.h"
#include "afxdialogex.h"
#include <EuhatTabCtrl.h>
#include <EuhatPostDefMfc.h>

EuhatPeerDlg::EuhatPeerDlg(EuhatBase *euhatBase)
	: EuhatRightDlg(IDD_EMPTY_CHILD_DIALOG, euhatBase)
{

}

EuhatPeerDlg::~EuhatPeerDlg()
{

}

void EuhatPeerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(EuhatPeerDlg, CDialogEx)
END_MESSAGE_MAP()

void EuhatPeerDlg::correctPos(CRect &rc)
{
	if (NULL == fmDlg_.get())
		return;

	EuhatRightDlg::correctPos(rc);

	fmDlg_->MoveWindow(rc);
}

BOOL EuhatPeerDlg::OnInitDialog()
{
	EuhatRightDlg::OnInitDialog();

	fmDlg_.reset(new FileManDlg(euhatBase_));
	fmDlg_->parentDlg_ = this;
	fmDlg_->create();

	euTabs_->add("FileMan");
	euTabs_->setCurSel(0);

	return TRUE;
}

void EuhatPeerDlg::show()
{
	fmDlg_->ShowWindow(SW_SHOW);
	fmDlg_->show();

	EuhatRightDlg::show();
}

void EuhatPeerDlg::hide()
{
	fmDlg_->ShowWindow(SW_HIDE);
	EuhatRightDlg::hide();
}

void EuhatPeerDlg::onClick(EuhatTabCtrl &ctrl)
{
	if (ctrl.getCurSel() == 0)
	{

	}
	else if (ctrl.getCurSel() == 1)
	{

	}
}