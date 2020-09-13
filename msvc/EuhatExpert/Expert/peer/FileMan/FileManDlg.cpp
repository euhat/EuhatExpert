#include "stdafx.h"
#include "../../Expert.h"
#include "../../ExpertDlg.h"
#include "FileManDlg.h"
#include "FileManCfgDlg.h"
#include "FileManSubDlg.h"
#include "FmFileDlg.h"
#include "../EuhatPeerDlg.h"
#include "../../EuhatRightDlg.h"
#include "../../EuhatLeftDlg.h"
#include <EuhatTabCtrl.h>
#include <OpCommonOs.h>
#include <EuhatOsType.h>
#include <EuhatPostDefMfc.h>

FileManDlg::FileManDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_FILE_MAN_DIALOG, euhatBase)
{
}

FileManDlg::~FileManDlg()
{
}

void FileManDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(FileManDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &FileManDlg::OnBnClickedBtnConnect)
END_MESSAGE_MAP()

void FileManDlg::correctPos()
{
	if (NULL == GetDlgItem(IDC_RADIO_ALERT))
		return;

	int margin = 8;
	int mainBtnWidth = 60;
	int btnHeight = 28;
	int radioBtnWidth = 100;
	int radioBtnHeight = 20;
	int cmdListHeight = 100;
	int scrollXHeight = EUHAT_SCROLL_BAR_DEFAULT_WIDTH;

	CRect rc;
	GetClientRect(&rc);

	CRect rcCmdList = rc;
	rcCmdList.left += margin;
	rcCmdList.right -= margin * 2 + radioBtnWidth;
	rcCmdList.bottom -= margin;
	rcCmdList.top = rcCmdList.bottom - cmdListHeight;
	listTask_->correctPos(rcCmdList);

	CRect rcMainBtn = rc;
	rcMainBtn.left = rc.right - radioBtnWidth - margin;
	rcMainBtn.top = rcCmdList.top;;
	rcMainBtn.right = rcMainBtn.left + mainBtnWidth;
	rcMainBtn.bottom = rcMainBtn.top + btnHeight;
	GetDlgItem(IDC_BTN_CONNECT)->MoveWindow(rcMainBtn);

	CRect rcRadio;
	rcRadio.left = rcMainBtn.left;
	rcRadio.right = rcRadio.left + radioBtnWidth;
	rcRadio.top = rcMainBtn.bottom + margin;
	rcRadio.bottom = rcRadio.top + radioBtnHeight;
	GetDlgItem(IDC_RADIO_ALERT)->MoveWindow(&rcRadio);
	rcRadio.top += radioBtnHeight;
	rcRadio.bottom = rcRadio.top + radioBtnHeight;
	GetDlgItem(IDC_RADIO_ALL_YES)->MoveWindow(&rcRadio);
	rcRadio.top += radioBtnHeight;
	rcRadio.bottom = rcRadio.top + radioBtnHeight;
	GetDlgItem(IDC_RADIO_ALL_NO)->MoveWindow(&rcRadio);

	subRc_.bottom = rcCmdList.top - scrollXHeight - margin;

	CRect subInnerRc;
	subDlg_->GetClientRect(&subInnerRc);
	subRc_.right = subRc_.left + subInnerRc.Width();

	CRect scrollXRc;
	scrollXRc = rc;
	scrollXRc.left = margin;
	scrollXRc.top = subRc_.bottom;
	scrollXRc.bottom = scrollXRc.top + scrollXHeight;
	scrollXRc.right -= margin;
	scrollX_->moveWindow(&scrollXRc);
	scrollX_->setRange(subInnerRc.Width(), scrollXRc.Width());

	CRect rcCfg;
	GetWindowRect(&rcCfg);
	euhatBase_->mainWnd_->ScreenToClient(&rcCfg);

	cfgDlg_->MoveWindow(rcCfg);
	cfgDlg_->BringWindowToTop();
}

BOOL FileManDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	cfgDlg_.reset(new FileManCfgDlg(euhatBase_));
	cfgDlg_->parent_ = this;
	string str = CT2A(parentDlg_->leftDlg_->peers_.getItem(parentDlg_->leftDlg_->getItem(parentDlg_))->str_);
	cfgDlg_->peerName_ = str;
	cfgDlg_->create();
	cfgDlg_->ShowWindow(SW_HIDE);

	listTask_.reset(new EuhatListCtrl(m_hWnd, this));
	listTask_->attr_ = EUHAT_LIST_CTRL_ATTR_MULTILINE | EUHAT_LIST_CTRL_ATTR_WHOLE_LINE;
	listTask_->cols_.height_ = 0;
	listTask_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Id"), EuhatListCtrl::Column::TypeInteger, 50));
	listTask_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Name"), EuhatListCtrl::Column::TypeText, 300));
	listTask_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Detail"), EuhatListCtrl::Column::TypeText, 300));
	listTask_->vscroller_.reset(new EuhatScrollVBar(m_hWnd, listTask_.get()));

	subDlg_.reset(new FileManSubDlg(euhatBase_));
	subDlg_->parentDlg_ = this;
	subDlg_->create(this);

	scrollX_.reset(new EuhatScrollHBar(m_hWnd, this));

	correctPos();

	return TRUE;
}

void FileManDlg::onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset)
{
	subRc_.MoveToX(-innerWindowOffset);
	subDlg_->MoveWindow(subRc_);
}

void FileManDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void FileManDlg::connect()
{
	cfgDlg_->ShowWindow(SW_SHOW);
	cfgDlg_->BringWindowToTop();
}

int FileManDlg::show()
{
	BringWindowToTop();
	if (cfgDlg_->IsWindowVisible())
	{
		cfgDlg_->BringWindowToTop();
	}
	return 1;
}

void FileManDlg::resetFileManClient()
{
	subDlg_->remote_.reset(new FileManClient(subDlg_->scheduler_.get()));
	subDlg_->scheduler_->remote_ = subDlg_->remote_.get();
	subDlg_->dlgRemote_->engine_ = subDlg_->remote_.get();

	subDlg_->remote_->setDbOp(euhatBase_->db_.get());

	subDlg_->remote_->serverIp_ = cfgDlg_->ip_;
	subDlg_->remote_->serverPort_ = cfgDlg_->port_;
	subDlg_->remote_->visitCode_ = cfgDlg_->visitCode_;

	shared_ptr<FmTaskConnect> task(new FmTaskConnect());
	task->retryTimes_ = 0;
	task->ip_ = cfgDlg_->ip_;
	task->port_ = cfgDlg_->port_;
	subDlg_->scheduler_->add(task);

	subDlg_->remote_->start();
}

void FileManDlg::dataToEngine()
{
	subDlg_->scheduler_->cleanAll();

	resetFileManClient();

	subDlg_->fmscOnRefresh();

	cfgDlg_->ShowWindow(SW_HIDE);
}

void FileManDlg::OnBnClickedBtnConnect()
{
	connect();
}
