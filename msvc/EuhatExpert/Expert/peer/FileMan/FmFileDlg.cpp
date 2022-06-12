#include "stdafx.h"
#include "../../Expert.h"
#include "FmFileDlg.h"
#include "FileManSubDlg.h"
#include "FileManDlg.h"
#include "FileManCfgDlg.h"
#include "../EuhatPeerDlg.h"
#include "../../EuhatRightDlg.h"
#include "../../EuhatLeftDlg.h"
#include "afxdialogex.h"
#include <EuhatOsType.h>
#include <OpCommonOs.h>
#include <EuhatPostDefMfc.h>

FmFileDlg::FmFileDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_FILE_MAN_FILE_DIALOG, euhatBase)
{
	engine_ = NULL;
	isLocal_ = 0;
}

FmFileDlg::~FmFileDlg()
{
	listCtrl_.reset();
}

void FmFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(FmFileDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ENTER_DIR, &FmFileDlg::OnBnClickedBtnEnterDir)
END_MESSAGE_MAP()

void FmFileDlg::correctPos()
{
	if (NULL == GetDlgItem(IDC_LIST_FILE))
		return;

	int margin = 8;

	CRect rc;
	GetClientRect(&rc);

	correctPos(rc, margin, listCtrl_.get());

	CRect rcTabs;
	GetDlgItem(IDC_BTN_GO_UP_FOLDER)->GetWindowRect(&rcTabs);
	rcTabs.right = rcTabs.left + euTabs_->getWidth();
	ScreenToClient(rcTabs);
	euTabs_->moveWindow(&rcTabs);
}

void FmFileDlg::correctPos(CRect &rc, int margin, EuhatListCtrl *ctrl)
{
	CRect rcList;
	GetDlgItem(IDC_LIST_FILE)->GetWindowRect(&rcList);
	ScreenToClient(&rcList);
	rcList.bottom = rc.bottom;

	ctrl->correctPos(rcList);
}

BOOL FmFileDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	if (isLocal_)
	{
		SetDlgItemText(IDC_STATIC_DIRECTORY, _T("Local Directory"));
	}

	listCtrl_.reset(new EuhatListCtrl(m_hWnd, this));
	listCtrl_->cols_.height_ = 20;
	listCtrl_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Name"), EuhatListCtrl::Column::TypeText, 300));
	listCtrl_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Size"), EuhatListCtrl::Column::TypeInteger, 90));
	listCtrl_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Type"), EuhatListCtrl::Column::TypeText, 90));
	listCtrl_->cols_.cols_.push_back(EuhatListCtrl::Column(_T("Time"), EuhatListCtrl::Column::TypeDateTime, 0));
	listCtrl_->vscroller_.reset(new EuhatScrollVBar(m_hWnd, listCtrl_.get()));

	euTabs_.reset(new EuhatTabCtrl(m_hWnd, this));
	euTabs_->kUnselText_ = RGB(0, 0, 0);
	euTabs_->kSelText_ = RGB(255, 255, 255);
	euTabs_->brushBack_.reset(new EuhatBrush(GetSysColor(COLOR_BTNFACE)));
	euTabs_->brushBackSel_.reset(new EuhatBrush(RGB(0, 0, 0)));
	euTabs_->penBack_.reset(new EuhatPen(RGB(0, 0, 155)));
	euTabs_->penBackSel_.reset(new EuhatPen(RGB(255, 255, 255)));
	euTabs_->add("Up");
	euTabs_->add("Del");
//	euTabs_->add("New Folder");

	return TRUE;
}

void FmFileDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void FmFileDlg::displayFiles(JyDataReadStream &ds)
{
	string oldCurDir = curDir_.toStr();
	curDir_.inStr(ds.getStr());

	string curDir = curDir_.toStr();
	CString cstr = utf8ToWstr(curDir.c_str()).c_str();
	SetDlgItemText(IDC_EDIT_CUR_DIR, cstr);

	iconPool_.reset(new EuhatIconSysPool());

	listCtrl_->rows_.clear();
	
	int folderCount = 0;
	int fileCount = 0;
	while (1)
	{
		string name;
		char type = ds.get<char>();
		if (type == 0)
		{
			folderCount++;
			name = ds.getStr();
		}
		else if (type == 1)
		{
			fileCount++;
			name = ds.getStr();
		}
		else
			break;

		listCtrl_->rows_.push_back(EuhatListCtrl::Row());
		EuhatListCtrl::Row &row = listCtrl_->rows_.back();
		row.height_ = 18;
		if (type == 1)
		{
			string extension = ".dat";
			string path = extension;
			vector<string> out;
			splitTokenString(name.c_str(), '.', out);
			if (out.size() > 1)
			{
				extension = "." + out[out.size() - 1];
				EuhatPath euPath = curDir_;
				euPath.goSub((out[out.size() - 2] + "." + out[out.size() - 1]).c_str());
				path = move(euPath.toStr());
			}
			EuhatIconSysPool::Basket *basket = iconPool_->pickFile(path.c_str(), extension.c_str());
			row.icon_ = basket->normal_.icon_;
			row.iconSelected_ = basket->selected_.icon_;
		}
		else
		{
			row.icon_ = iconPool_->folderBasket_.normal_.icon_;
			row.iconSelected_ = iconPool_->folderBasket_.selected_.icon_;
		}

		row.cells_.push_back(EuhatListCtrl::Cell(utf8ToWstr(name.c_str()).c_str()));

		if (type == 1)
		{
			int64_t size = ds.get<int64_t>();
			row.cells_.push_back(EuhatListCtrl::Cell(size));

			string fileType = whFileType(name.c_str());
			row.cells_.push_back(EuhatListCtrl::Cell(utf8ToWstr(fileType.c_str()).c_str()));

			int64_t t = ds.get<int64_t>();
			row.cells_.push_back(EuhatListCtrl::Cell(t));
		}
	}
	DBG(("get file list end.\n"));

	EuhatRect rc;
	::GetClientRect(listCtrl_->hwnd_, &rc.rc_);
	listCtrl_->vscroller_->setRange(listCtrl_->getLogicalHeight(), rc.height());
	if (oldCurDir != curDir)
		listCtrl_->vscroller_->setPos(0);
	listCtrl_->cols_.cols_.front().isAscending_ = 1;
	listCtrl_->onLBtnDown(0, EUHAT_LIST_CTRL_CHECKBOX_WIDTH * 2, listCtrl_->cols_.height_ / 2);
	listCtrl_->onLBtnUp(0, EUHAT_LIST_CTRL_CHECKBOX_WIDTH * 2, listCtrl_->cols_.height_ / 2);

	char buf[1024];
	sprintf(buf, "%d folder(s), %d file(s)", folderCount, fileCount);
	cstr = CA2T(buf);
	SetDlgItemText(IDC_STATIC_DESCRIPTION, cstr);
	::InvalidateRect(listCtrl_->hwnd_, NULL, 0);
}

void FmFileDlg::onListCtrlDbClick(EuhatListCtrl *listCtrl, WPARAM wParam, int x, int y)
{
	EuhatListCtrl::Row *row = listCtrl->getFirstSelectedRow();
	if (NULL == row)
		return;

	if (row->cells_.size() > 1)
	{
		if (isLocal_)
		{
			string fileName = wstrToUtf8(row->cells_.front().value_.get());
			EuhatPath euPath = curDir_;
			euPath.goSub(fileName.c_str());
			CString szFile = utf8ToWstr(euPath.toStr().c_str()).c_str();
			HINSTANCE hInstRet = ShellExecute(NULL, _T("open"), szFile, NULL, NULL, SW_SHOWNORMAL);
			if (hInstRet < (HINSTANCE)32)
			{
				CString msg;
				msg.Format(_T("open [%s] failed, error code: %p."), szFile, hInstRet);
				MessageBox(msg);
			}
		}
		return;
	}
	string fileName = wstrToUtf8(row->cells_.front().value_.get());
	EuhatPath euPath = curDir_;
	euPath.goSub(fileName.c_str());
	parentDlg_->browserDir(engine_, euPath.toStr().c_str());
}

void FmFileDlg::OnBnClickedBtnGoUpFolder()
{
	EuhatPath euPath = curDir_;
	euPath.goUp();
	parentDlg_->browserDir(engine_, euPath.toStr().c_str());
}

void FmFileDlg::onClick(EuhatTabCtrl &ctrl)
{
	int curSel = -1;
	if (ctrl.getCurSel() == 0)
	{
		OnBnClickedBtnGoUpFolder();
	}
	else if (ctrl.getCurSel() == 1)
	{
		parentDlg_->delFile(engine_);
	}
	else if (ctrl.getCurSel() == 2)
	{
		parentDlg_->newFolder(engine_);
	}
	ctrl.setCurSel(-1);
}

void FmFileDlg::OnBnClickedBtnEnterDir()
{
	CString cstr;
	GetDlgItemTextW(IDC_EDIT_CUR_DIR, cstr);
	parentDlg_->browserDir(engine_, wstrToUtf8(cstr).c_str());
}
