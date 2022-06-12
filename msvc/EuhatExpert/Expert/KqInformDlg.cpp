#include "stdafx.h"
#include "Expert.h"
#include "ExpertDlg.h"
#include "KqInformDlg.h"
#include "afxdialogex.h"
#include <EuhatPostDefMfc.h>

#define WM_USER_KQ_DELETE (WM_USER + 1)

static int time2Minutes(const char *tstr)
{
	vector<string> out;
	splitTokenString(tstr, ':', out);
	if (out.size() != 3)
		return 0;
	return atoi(out[0].c_str()) * 60 + atoi(out[1].c_str());
}

static time_t minutes2Time(int slot)
{
	time_t t = time(NULL);
	tm *local = localtime(&t);
	local->tm_hour = slot / 60;
	local->tm_min = slot % 60;
	local->tm_sec = 0;
	return mktime(local);
}

static WH_THREAD_DEF(displayOpThread, arg)
{
	WH_THREAD_PREPROCESS;

	KqInformDlg *pThis = (KqInformDlg *)arg;
	pThis->displayOpProc();
	return 0;
}

int KqInformDlg::displayOpProc()
{
	Create(KqInformDlg::IDD, NULL);
	ShowWindow(SW_SHOW);

	MSG msg;
	int ret;
	while (ret = GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (ret == -1)
			break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	isOpened_ = 0;
	return 1;
}

int KqInformDlg::open()
{
	if (isOpened_)
		return 0;

	isOpened_ = 1;
	whThreadCreate(displayOpThreadHandle_, displayOpThread, this);
	return 1;
}

int KqInformDlg::close()
{
	if (!isOpened_)
		return 0;

	isOpened_ = 0;
	PostMessage(WM_CLOSE);
	whThreadJoin(displayOpThreadHandle_);
	return 1;
}

int KqInformDlg::check()
{
	time_t t = time(NULL);
	tm *local = localtime(&t);
	int slot = local->tm_hour * 60 + local->tm_min;

	for (JyList<WorkSpan>::iterator it = euhatBase_->workSpans_.list_.begin(); it != euhatBase_->workSpans_.list_.end(); it++)
	{
		int startMin = time2Minutes(it->startTime_.c_str());
		int beginMin = startMin - it->beginSecs_ / 60;
		int endMin = time2Minutes(it->endTime_.c_str());
		int untilMin = endMin + it->untilSecs_ / 60;

		if (beginMin <= slot && slot <= endMin)
		{
			time_t dispTime = minutes2Time(startMin);
			if (dispTime_ != dispTime)
			{
				dispTime_ = dispTime;
				euhatBase_->mainWnd_->PostMessage(WM_TIMER, EU_TIMER_ID_KQ_INFORM);
				break;
			}
			if (!hasInChecked_)
			{
				open();
				isInOrOut_ = 1;
			}
			return 1;
		}
		else if (endMin <= slot && slot <= untilMin)
		{
			time_t dispTime = minutes2Time(endMin);
			if (dispTime_ != dispTime)
			{
				dispTime_ = dispTime;
				euhatBase_->mainWnd_->PostMessage(WM_TIMER, EU_TIMER_ID_KQ_INFORM);
				break;
			}
			if (!hasOutChecked_)
			{
				open();
				isInOrOut_ = 0;
			}
			return 1;
		}
	}

	hasInChecked_ = 0;
	hasOutChecked_ = 0;
	isInOrOut_ = 1;
	close();

	return 1;
}

KqInformDlg::KqInformDlg(EuhatBase *euhatBase)
	: MfcBasePage(KqInformDlg::IDD, euhatBase)
{
	isOpened_ = 0;
	hasInChecked_ = 0;
	hasOutChecked_ = 0;
	isInOrOut_ = 1;

	dispTime_ = 0;
}

KqInformDlg::~KqInformDlg()
{

}

void KqInformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(KqInformDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &KqInformDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &KqInformDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void KqInformDlg::OnPaint()
{
	CPaintDC dc(this);
}

BOOL KqInformDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	pushUiLoc("NOTIFICATION_MSG", "Have check at %d:%d?");

	string tag = euhatBase_->tagApp_ + "/" + "KqInformDlg";
	euhatBase_->refreshUiLocalization(this, tag.c_str());

	refreshUiLocalization();

	//CenterWindow();
	int cx = GetSystemMetrics(SM_CXMAXIMIZED);
	int cy = GetSystemMetrics(SM_CYMAXIMIZED);
	CRect rc;
	GetWindowRect(&rc);
	rc.MoveToXY(cx - rc.Width(), cy - rc.Height());
	MoveWindow(rc);

	return TRUE;
}

int KqInformDlg::refreshUiLocalization()
{
	if (NULL == m_hWnd)
		return 0;

	char msg[1024];
	CString formatCStr = t("NOTIFICATION_MSG");
	string formatStr = CT2A(formatCStr);

	tm *local = localtime(&dispTime_);

	sprintf(msg, formatStr.c_str(), local->tm_hour, local->tm_min);
	CString cstr = CA2T(msg);
	
	SetDlgItemText(IDC_STATIC_INFORM_TXT, cstr);

	return 1;
}

void KqInformDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();

//	delete this;
}

void KqInformDlg::OnBnClickedOk()
{
	if (isInOrOut_)
		hasInChecked_ = 1;
	else
		hasOutChecked_ = 1;

	PostMessage(WM_CLOSE);
}

void KqInformDlg::OnBnClickedCancel()
{
	PostMessage(WM_CLOSE);
}

void KqInformDlg::OnClose()
{
	MfcBasePage::OnClose();

	DestroyWindow();
}

void KqInformDlg::OnDestroy()
{
	MfcBasePage::OnDestroy();

	PostQuitMessage(0);
}
