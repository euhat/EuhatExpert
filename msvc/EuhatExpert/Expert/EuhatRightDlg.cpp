#include "stdafx.h"
#include "Expert.h"
#include "ExpertDlg.h"
#include "EuhatRightDlg.h"
#include "afxdialogex.h"
#include <EuhatTabCtrl.h>
#include <EuhatPostDefMfc.h>

EuhatRightDlg::EuhatRightDlg(int idd, EuhatBase *euhatBase)
	: MfcBasePage(idd, euhatBase)
{
}

EuhatRightDlg::~EuhatRightDlg()
{

}

void EuhatRightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(EuhatRightDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void EuhatRightDlg::correctPos(CRect &rc)
{
	int width = rc.right - rc.left;
	CRect rcTabs;
	int widthTabs = euTabs_->getWidth();
	rcTabs.left = EU_EXPERT_DLG_LEFT_WIDTH + (width - widthTabs) / 2;
	rcTabs.right = rcTabs.left + euTabs_->getWidth();
	rcTabs.top = EU_EXPERT_DLG_MARGIN;
	rcTabs.bottom = rcTabs.top + EU_EXPERT_DLG_TITLE_HEIGHT;
	::MoveWindow(euTabs_->hwnd_, rcTabs.left, rcTabs.top, rcTabs.right - rcTabs.left, rcTabs.bottom - rcTabs.top, 1);
	::BringWindowToTop(euTabs_->hwnd_);
}

BOOL EuhatRightDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	euTabs_.reset(new EuhatTabCtrl(euhatBase_->mainWnd_->m_hWnd, this));

	ShowWindow(SW_HIDE);
	isVisible_ = 0;

	return TRUE;
}

void EuhatRightDlg::show()
{
	::BringWindowToTop(euTabs_->hwnd_);
	::ShowWindow(euTabs_->hwnd_, SW_SHOW);
	isVisible_ = 1;
}

void EuhatRightDlg::hide()
{
	::ShowWindow(euTabs_->hwnd_, SW_HIDE);
	isVisible_ = 0;
}

void EuhatRightDlg::onClick(EuhatTabCtrl &ctrl)
{

}