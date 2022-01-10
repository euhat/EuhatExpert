#include "stdafx.h"
#include "../../Expert.h"
#include "../../ExpertDlg.h"
#include "FileManDlg.h"
#include "FileManCfgDlg.h"
#include "FileManSubDlg.h"
#include "../EuhatPeerDlg.h"
#include "../../EuhatRightDlg.h"
#include "../../EuhatLeftDlg.h"
#include "FmFileDlg.h"
#include <app/FileMan/client/FileManClient.h>
#include <app/FileMan/common/FmLocal.h>
#include <app/FileMan/common/FmScheduler.h>
#include <app/FileMan/common/FmTask.h>
#include <EuhatPostDefMfc.h>

#define WM_USER_FILE_MAN_CLIENT_NOTIFY (WM_USER + 1)

FileManSubDlg::FileManSubDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_FILE_MAN_SUB_DIALOG, euhatBase)
{
}

FileManSubDlg::~FileManSubDlg()
{
	remote_.reset();
}

void FileManSubDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(FileManSubDlg, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_FILE_MAN_CLIENT_NOTIFY, &FileManSubDlg::onFileManClientNotify)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &FileManSubDlg::OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_REMOTE_COPY_TO_LOCAL, &FileManSubDlg::OnBnClickedBtnRemoteCopyToLocal)
	ON_BN_CLICKED(IDC_BTN_LOCAL_COPY_TO_REMOTE, &FileManSubDlg::OnBnClickedBtnLocalCopyToRemote)
	ON_BN_CLICKED(IDC_BTN_REMOTE_MOVE_TO_LOCAL, &FileManSubDlg::OnBnClickedBtnRemoteMoveToLocal)
	ON_BN_CLICKED(IDC_BTN_LOCAL_MOVE_TO_REMOTE, &FileManSubDlg::OnBnClickedBtnLocalMoveToRemote)
	ON_BN_CLICKED(IDC_BTN_COPY_CLIPBOARD_TO_REMOTE, &FileManSubDlg::OnBnClickedBtnCopyClipboardToRemote)
	ON_BN_CLICKED(IDC_BTN_COPY_CLIPBOARD_FROM_REMOTE, &FileManSubDlg::OnBnClickedBtnCopyClipboardFromRemote)
END_MESSAGE_MAP()

void FileManSubDlg::fmscNotify()
{
	PostMessage(WM_USER_FILE_MAN_CLIENT_NOTIFY);
}

LRESULT FileManSubDlg::onFileManClientNotify(WPARAM wParam, LPARAM lParam)
{
	if (NULL == scheduler_.get())
		return 0;
	scheduler_->user_->notify();
	return 1;
}

void FileManSubDlg::correctPos()
{
	if (NULL == GetDlgItem(IDC_LIST_REMOTE_FILE_LIST))
		return;

	int margin = 8;
	int mainBtnWidth = 60;
	int btnHeight = 28;
	int cmdListHeight = 100;

	CRect rc;
	GetClientRect(&rc);

	correctPos(rc, margin, IDC_LIST_REMOTE_FILE_LIST, dlgRemote_.get());
	correctPos(rc, margin, IDC_LIST_LOCAL_FILE_LIST, dlgLocal_.get());

	CRect rcCfg;
	GetWindowRect(&rcCfg);
	euhatBase_->mainWnd_->ScreenToClient(&rcCfg);
}

void FileManSubDlg::correctPos(CRect &rc, int margin, int idd, MfcBasePage *dlg)
{
	CRect rcList;
	GetDlgItem(idd)->GetWindowRect(&rcList);
	ScreenToClient(&rcList);
	rcList.bottom = rc.bottom - margin;
	dlg->MoveWindow(rcList);
}

BOOL FileManSubDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	scheduler_.reset(new FmScheduler(this));
	local_.reset(new FmLocal(scheduler_.get()));
	scheduler_->local_ = local_.get();
	local_->start();

	dlgRemote_.reset(new FmFileDlg(euhatBase_));
	dlgRemote_->parentDlg_ = this;
	dlgRemote_->create(this);

	dlgLocal_.reset(new FmFileDlg(euhatBase_));
	dlgLocal_->engine_ = local_.get();
	dlgLocal_->isLocal_ = 1;
	dlgLocal_->parentDlg_ = this;
	dlgLocal_->create(this);

	browserDir(local_.get(), "");

	correctPos();

	return TRUE;
}

void FileManSubDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void FileManSubDlg::browserDir(JyMsgLoop *loop, const char *dir)
{
	scheduler_->cleanError();

	shared_ptr<FmTaskGetSubList> task(new FmTaskGetSubList(loop == remote_.get() ? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
	task->path_.reset(opStrDup(dir));
	task->action_ = FmTaskGetSubList::ActionInformUi;
	scheduler_->add(task);
	scheduler_->notifyEngine();
	fmscOnRefresh();
}

void FileManSubDlg::fmscOnGetSubList(JyMsgLoop *loop, JyDataReadStream &ds)
{
	if (loop == remote_.get())
	{
		dlgRemote_->displayFiles(ds);
	}
	else
	{
		dlgLocal_->displayFiles(ds);
	}
}

void FileManSubDlg::fmscNeedReconnect()
{
	parentDlg_->resetFileManClient();
	scheduler_->notifyEngine();
	scheduler_->notifyUi();
}

void FileManSubDlg::fmscOnRefresh()
{
	onClipboardFileComing();

	parentDlg_->listTask_->rows_.clear();

	WhMutexGuard guard(&scheduler_->mutex_);
	if (scheduler_->tasks_.size() == 0 && NULL != scheduler_->refreshSubListTask_.get())
	{
		scheduler_->tasks_.push_back(scheduler_->refreshSubListTask_);
		scheduler_->refreshSubListTask_.reset();
		scheduler_->notifyEngine();
		scheduler_->notifyUi();
		return;
	}
	int idx = 0;
	for (list<shared_ptr<FmTask>>::iterator it = scheduler_->tasks_.begin(); it != scheduler_->tasks_.end(); it++, idx++)
	{
		FmTask *t = (*it).get();

		parentDlg_->listTask_->rows_.push_back(EuhatListCtrl::Row());
		EuhatListCtrl::Row &row = parentDlg_->listTask_->rows_.back();
		row.height_ = 18;
		row.cells_.push_back(EuhatListCtrl::Cell(t->id_));
		string name;
		string detail;
		if (t->type_ == FmTask::TypeGetSubList || t->type_ == FmTask::TypeGetPeerSubList)
		{
			FmTaskGetSubList *task = (FmTaskGetSubList *)t;
			name = task->path_.get();
			if (task->result_ == FmResultOk)
				detail = "refreshing...";
			else
				detail = "open failed.";
		}
		else if (t->type_ == FmTask::TypeDownload)
		{
			FmTaskDownload *task = (FmTaskDownload *)t;
			name = task->localPath_.get();
			if (task->offset_ < 0)
				detail = "downloading...";
			else
			{
				if (task->result_ == FmResultOk)
				{
					double percent = 100.0 * (task->offset_ + task->eatGuess_) / task->totalFileSize_;
					char buf[1024];
					sprintf(buf, "downloading %.02f%%", (float)percent);
					detail = buf;
				}
				else
				{
					detail = "downloading failed.";
				}
			}
		}
		else if (t->type_ == FmTask::TypeUpload)
		{
			FmTaskUpload *task = (FmTaskUpload *)t;
			name = task->localPath_.get();
			if (task->offset_ < 0)
				detail = "uploading...";
			else
			{
				if (task->result_ == FmResultOk)
				{
					double percent = 100.0 * (task->offset_ + task->eatGuess_) / task->totalFileSize_;
					char buf[1024];
					sprintf(buf, "uploading %.02f%%", (float)percent);
					detail = buf;
				}
				else
				{
					detail = "uploading failed.";
				}
			}
		}
		else if (t->type_ == FmTask::TypeCopy || t->type_ == FmTask::TypeCopyPeer)
		{
			FmTaskCopy *task = (FmTaskCopy *)t;
			name = task->toPath_.get();
			detail = "copying...";
		}
		else if (t->type_ == FmTask::TypeDel || t->type_ == FmTask::TypeDelPeer)
		{
			FmTaskDel *task = (FmTaskDel *)t;
			name = task->path_.get();
			if (task->result_ != FmResultOk)
				detail = "deleting failed.";
			else
				detail = "deleting...";
		}
		else if (t->type_ == FmTask::TypeNew || t->type_ == FmTask::TypeNewPeer)
		{
			FmTaskNew *task = (FmTaskNew *)t;
			name = task->path_.get();
			if (task->result_ != FmResultOk)
				detail = "creating failed.";
			else
				detail = "creating...";
		}
		else if (t->type_ == FmTask::TypeConnect)
		{
			FmTaskConnect *task = (FmTaskConnect *)t;
			name = "Connecting " + task->ip_ + ":" + intToString(task->port_) + "...";
			if (task->retryTimes_ > 0)
			{
				detail = "retry times: ";
				detail += intToString(task->retryTimes_);
			}
			else if (task->retryTimes_ < 0)
			{
				if (task->result_ == FmResultVisitCodeMismatch)
					detail = "visit code mismatch.";
				else
					detail = "connected.";
			}
		}
		if (idx > 100)
		{
			char buf[1024];
			sprintf(buf, "%d tasks left not shown...", (int)(scheduler_->tasks_.size() - idx));
			row.cells_.push_back(EuhatListCtrl::Cell(utf8ToWstr(buf).c_str()));
			break;
		}
		row.cells_.push_back(EuhatListCtrl::Cell(utf8ToWstr(name.c_str()).c_str()));
		row.cells_.push_back(EuhatListCtrl::Cell(utf8ToWstr(detail.c_str()).c_str()));
	}

	EuhatRect rc;
	::GetClientRect(parentDlg_->listTask_->hwnd_, &rc.rc_);
	parentDlg_->listTask_->vscroller_->setRange(parentDlg_->listTask_->getLogicalHeight(), rc.height());

	::InvalidateRect(parentDlg_->listTask_->hwnd_, NULL, 0);
}

void FileManSubDlg::fmscOnGetSysInfo(JyMsgLoop *loop, JyDataReadStream &ds)
{
	char *hostName = ds.getStr();
	dlgRemote_->curDir_.isUnix_ = ds.get<short>();
	remote_->isUnix_ = dlgRemote_->curDir_.isUnix_;
	browserDir(loop, ds.getStr());

	FileManDlg *mainDlg = parentDlg_;
	mainDlg->cfgDlg_->saveData(hostName);
	mainDlg->cfgDlg_->dataToUi();
	mainDlg->parentDlg_->leftDlg_->modifyItem(mainDlg->parentDlg_, hostName);
}

void FileManSubDlg::OnBnClickedBtnConnect()
{
	parentDlg_->connect();
}

void FileManSubDlg::newFolder(JyMsgLoop *loop)
{

}

void FileManSubDlg::delFile(JyMsgLoop *loop)
{
	scheduler_->cleanError();

	EuhatPath parentDir;
	EuhatListCtrl *listCtrl;
	if (loop == remote_.get())
	{
		parentDir = dlgRemote_->curDir_;
		listCtrl = dlgRemote_->listCtrl_.get();
	}
	else
	{
		parentDir = dlgLocal_->curDir_;
		listCtrl = dlgLocal_->listCtrl_.get();
	}
	if (!parentDir.isUnix_ && parentDir.path_.size() <= 0)
		return;
	int isToGo = 0;
	for (auto it = listCtrl->rows_.begin(); it != listCtrl->rows_.end(); it++)
	{
		if (!it->isSelected_)
			continue;
		if (!isToGo)
		{
			if (IDOK == MessageBox(_T("Are you sure to delete?"), _T("Warning"), MB_OKCANCEL))
			{
				isToGo = 1;
			}
			else
			{
				return;
			}
		}
		string fileName = wstrToUtf8(it->cells_.front().value_.get());
		if (it->cells_.size() <= 1)
		{
			shared_ptr<FmTaskGetSubList> task(new FmTaskGetSubList(loop == remote_.get() ? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
			parentDir.goSub(fileName.c_str());
			task->path_.reset(opStrDup(parentDir.toStr().c_str()));
			parentDir.goUp();
			task->action_ = FmTaskGetSubList::ActionDel;
			scheduler_->add(task);
		}
		else
		{
			shared_ptr<FmTaskDel> task(new FmTaskDel(loop == remote_.get()? FmTask::TypeDelPeer : FmTask::TypeDel));
			parentDir.goSub(fileName.c_str());
			task->path_.reset(opStrDup(parentDir.toStr().c_str()));
			parentDir.goUp();
			task->isFolder_ = 0;
			task->priority_ = 100;
			scheduler_->add(task);
		}
	}

	shared_ptr<FmTaskGetSubList> task(new FmTaskGetSubList(loop == remote_.get() ? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
	task->path_.reset(opStrDup(parentDir.toStr().c_str()));
	task->action_ = FmTaskGetSubList::ActionInformUi;
	scheduler_->addRefreshTask(task);

	scheduler_->notifyEngine();
	fmscOnRefresh();
}

void FileManSubDlg::transfer(int isDownload, int isMove)
{
	scheduler_->cleanError();

	EuhatPath euLocal = dlgLocal_->curDir_;
	if (!euLocal.isUnix_ && euLocal.path_.size() <= 0)
		return;
	EuhatPath euPeer = dlgRemote_->curDir_;
	if (!euPeer.isUnix_ && euPeer.path_.size() <= 0)
		return;
	EuhatListCtrl *listFrom;
	EuhatListCtrl *listTo;
	if (isDownload)
	{
		listFrom = dlgRemote_->listCtrl_.get();
		listTo = dlgLocal_->listCtrl_.get();
	}
	else
	{
		listFrom = dlgLocal_->listCtrl_.get();
		listTo = dlgRemote_->listCtrl_.get();
	}

	list<EuhatListCtrl::Row *> rowsFrom;
	list<EuhatListCtrl::Row *> rowsAnd;

	for (auto it = listFrom->rows_.begin(); it != listFrom->rows_.end(); it++)
	{
		if (!it->isSelected_)
			continue;
		rowsFrom.push_back(&*it);
	}
	for (auto itTo = listTo->rows_.begin(); itTo != listTo->rows_.end(); itTo++)
	{
		for (auto itFrom = rowsFrom.begin(); itFrom != rowsFrom.end(); itFrom++)
		{
			wchar_t *strFrom = (*itFrom)->cells_.front().value_.get();
			wchar_t *strTo = itTo->cells_.front().value_.get();
			if (opStrCmpNoCase(strFrom, strTo) == 0)
			{
				rowsAnd.push_back(*itFrom);
			}
		}
	}
	if (rowsAnd.size() > 0)
	{
		wstring txt = L"Below files will be overwritten, Are you sure?\n";
		int idx = 0;
		list<EuhatListCtrl::Row *>::iterator it;
		for (it = rowsAnd.begin(); it != rowsAnd.end() && idx < 5; it++, idx++)
		{
			txt += L"\n\t";
			txt += (*it)->cells_.front().value_.get();
		}
		if (it != rowsAnd.end())
			txt += L"\n\t...";
		if (IDOK != MessageBox(txt.c_str(), L"Warning", MB_OKCANCEL))
		{
			return;
		}
	}
	for (auto it = rowsFrom.begin(); it != rowsFrom.end(); it++)
	{
		if (!(*it)->isSelected_)
			continue;
		string fileName = wstrToUtf8((*it)->cells_.front().value_.get());
		if ((*it)->cells_.size() <= 1)
		{
			shared_ptr<FmTaskGetSubList> task(new FmTaskGetSubList(isDownload? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
			euLocal.goSub(fileName.c_str());
			if (isDownload)
				task->peerPath_.reset(opStrDup(euLocal.toStr().c_str()));
			else
				task->path_.reset(opStrDup(euLocal.toStr().c_str()));
			euLocal.goUp();
			euPeer.goSub(fileName.c_str());
			if (isDownload)
				task->path_.reset(opStrDup(euPeer.toStr().c_str()));
			else
				task->peerPath_.reset(opStrDup(euPeer.toStr().c_str()));
			euPeer.goUp();
			task->action_ = FmTaskGetSubList::ActionDownUp;
			task->isMove_ = isMove;
			scheduler_->add(task);
		}
		else
		{
			shared_ptr<FmTaskDownload> task(isDownload ? new FmTaskDownload() : new FmTaskUpload());
			euLocal.goSub(fileName.c_str());
			task->localPath_.reset(opStrDup(euLocal.toStr().c_str()));
			euLocal.goUp();
			euPeer.goSub(fileName.c_str());
			task->peerPath_.reset(opStrDup(euPeer.toStr().c_str()));
			euPeer.goUp();
			task->isMove_ = isMove;
			auto cellIt = (*it)->cells_.begin();
			cellIt++;
			task->totalFileSize_ = cellIt->i_;
			cellIt++;
			cellIt++;
			task->lastWriteTime_ = cellIt->i_;
			task->offset_ = -1;
			scheduler_->add(task);
		}
	}

	shared_ptr<FmTaskGetSubList> task(new FmTaskGetSubList(isDownload ? FmTask::TypeGetSubList : FmTask::TypeGetPeerSubList));
	if (isDownload)
		task->path_.reset(opStrDup(euLocal.toStr().c_str()));
	else
		task->path_.reset(opStrDup(euPeer.toStr().c_str()));
	task->action_ = FmTaskGetSubList::ActionInformUi;
	scheduler_->addRefreshTask(task);

	scheduler_->notifyEngine();
	fmscOnRefresh();
}

void FileManSubDlg::OnBnClickedBtnRemoteCopyToLocal()
{
	transfer(1, 0);
}

void FileManSubDlg::OnBnClickedBtnLocalCopyToRemote()
{
	transfer(0, 0);
}

void FileManSubDlg::OnBnClickedBtnRemoteMoveToLocal()
{
	transfer(1, 1);
}

void FileManSubDlg::OnBnClickedBtnLocalMoveToRemote()
{
	transfer(0, 1);
}

void FileManSubDlg::OnBnClickedBtnCopyClipboardToRemote()
{
	EuhatPath localPath;
	localPath.isUnix_ = dlgLocal_->curDir_.isUnix_;
	string dir = "clipboard";
	localPath.goSub(dir.c_str());

	string fileName = "client.txt";
	localPath.goSub(fileName.c_str());

	opMkDir(dir.c_str());
	string localPathStr = localPath.toStr();

	HWND hWnd = GetSafeHwnd();
	::OpenClipboard(hWnd);
	HANDLE hClipMemory = ::GetClipboardData(CF_UNICODETEXT);
	SIZE_T dwLength = GlobalSize(hClipMemory);
	LPBYTE lpClipMemory = (LPBYTE)GlobalLock(hClipMemory);
	string utf8Buf = wstrToUtf8((wchar_t *)lpClipMemory);
	memToFile(localPathStr.c_str(), (char*)utf8Buf.c_str(), (int)utf8Buf.length());
	GlobalUnlock(hClipMemory);
	::CloseClipboard();

	EuhatPath remotePath;
	remotePath.isUnix_ = dlgRemote_->curDir_.isUnix_;
	remotePath.goSub(dir.c_str());
	remotePath.goSub(fileName.c_str());

	shared_ptr<FmTaskDownload> task(new FmTaskUpload());
	task->localPath_.reset(opStrDup(localPathStr.c_str()));
	task->peerPath_.reset(opStrDup(remotePath.toStr().c_str()));
	task->totalFileSize_ = whGetFileSize(localPathStr.c_str());
	task->lastWriteTime_ = time(NULL);
	task->offset_ = -1;
	scheduler_->add(task);

	scheduler_->notifyEngine();
	fmscOnRefresh();
}

void FileManSubDlg::OnBnClickedBtnCopyClipboardFromRemote()
{
	EuhatPath localPath;
	localPath.isUnix_ = dlgLocal_->curDir_.isUnix_;
	string dir = "clipboard";
	localPath.goSub(dir.c_str());

	string fileName = "server.txt";
	localPath.goSub(fileName.c_str());

	opMkDir(dir.c_str());
	string localPathStr = localPath.toStr();

	opUnlink(localPathStr.c_str());

	EuhatPath remotePath;
	remotePath.isUnix_ = dlgRemote_->curDir_.isUnix_;
	remotePath.goSub(dir.c_str());
	remotePath.goSub(fileName.c_str());

	shared_ptr<FmTaskDownload> task(new FmTaskDownload());
	task->localPath_.reset(opStrDup(localPathStr.c_str()));
	task->peerPath_.reset(opStrDup(remotePath.toStr().c_str()));
	task->totalFileSize_ = 0;
	task->lastWriteTime_ = time(NULL);
	task->offset_ = -1;
	scheduler_->add(task);

	scheduler_->notifyEngine();
	fmscOnRefresh();
}

void FileManSubDlg::onClipboardFileComing()
{
	EuhatPath localPath;
	localPath.isUnix_ = dlgLocal_->curDir_.isUnix_;
	string dir = "clipboard";
	localPath.goSub(dir.c_str());

	string fileName = "server.txt";
	localPath.goSub(fileName.c_str());

	opMkDir(dir.c_str());
	string localPathStr = localPath.toStr();
	if (!doesFileExist(localPathStr.c_str()))
		return;

	unsigned int len;
	unique_ptr<char[]> buf(memFromWholeFile(localPathStr.c_str(), &len));

	wstring wstrBuf = utf8ToWstr(buf.get());
	len = (unsigned int)((wstrBuf.length() + 1) * sizeof(wchar_t));

	HANDLE hGlobalMemory = GlobalAlloc(GHND, len + 1);
	LPBYTE lpGlobalMemory = (LPBYTE)GlobalLock(hGlobalMemory);
	memcpy(lpGlobalMemory, wstrBuf.c_str(), len);
	GlobalUnlock(hGlobalMemory);

	HWND hWnd = GetSafeHwnd();
	::OpenClipboard(hWnd);
	::EmptyClipboard();
	::SetClipboardData(CF_UNICODETEXT, hGlobalMemory);
	::CloseClipboard();

	opUnlink(localPathStr.c_str());
}