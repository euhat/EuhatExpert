#include "stdafx.h"
#include "../../Expert.h"
#include "../../base/EuhatWtPayload.h"
#include "WorkTraceDlg.h"
#include "afxdialogex.h"
#include <EuhatTabCtrl.h>
#include <EuhatOsType.h>
#include <time.h>
#include <EuhatPostDefMfc.h>

WorkTraceDlg::WorkTraceDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_WORK_TRACE_DIALOG, euhatBase)
{
}

WorkTraceDlg::~WorkTraceDlg()
{
}

void WorkTraceDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALENDAR_WT, calendarWt_);
}

BEGIN_MESSAGE_MAP(WorkTraceDlg, CDialogEx)
	ON_WM_SIZING()
	ON_WM_DESTROY()
	ON_NOTIFY(MCN_SELCHANGE, IDC_CALENDAR_WT, &WorkTraceDlg::OnSelchangeCalendarWt)
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()

#define WORK_TRACE_MAGIN 8
#define EUHAT_CHART_WIN_WIDTH 800

BOOL WorkTraceDlg::OnInitDialog()
{
	time_t t = time(NULL);
	char filePath[256];
	tm *local = localtime(&t);
	strftime(filePath, 254, EUHAT_WT_PL_FILE_PATH_PATTERN, local);
	DBG(("open payload file [%s].\n", filePath));

	euhatChart_.reset(new WorkTraceChart(m_hWnd, euhatBase_));
	euhatChart_->read(filePath);
	scrollChartX_.reset(new EuhatScrollHBar(m_hWnd, this));

	MfcBasePage::OnInitDialog();

	correctPos();

	SetTimer(0, 30 * 1000, NULL);

	return TRUE;
}

void WorkTraceDlg::correctPos()
{
	if (NULL == calendarWt_.m_hWnd)
		return;

	RECT rect;
	GetWindowRect(&rect);

	int chartWidth = rect.right - rect.left;
	if (chartWidth > EUHAT_CHART_WIDTH + 1)
		chartWidth = EUHAT_CHART_WIDTH + 1;

	RECT chartRect;
	chartRect.left = 0;
	chartRect.top = 0;
	chartRect.right = chartRect.left + chartWidth;
	chartRect.bottom = chartRect.top + EUHAT_CHART_X_AXIS_HEIGHT + EUHAT_CHART_MARK_HEIGHT;
	::MoveWindow(euhatChart_->hwnd_, chartRect.left, chartRect.top, chartRect.right - chartRect.left, chartRect.bottom - chartRect.top, 1);

	RECT scrollChartXRect;
	scrollChartXRect.left = chartRect.left;
	scrollChartXRect.top = chartRect.bottom;
	scrollChartXRect.right = chartRect.right;
	scrollChartXRect.bottom = scrollChartXRect.top + EUHAT_SCROLL_BAR_DEFAULT_WIDTH;
	scrollChartX_->moveWindow(&scrollChartXRect);
	scrollChartX_->setRange(EUHAT_CHART_WIDTH, chartWidth);

	RECT calendarWtRect;
	calendarWtRect.left = 0;
	calendarWtRect.top = scrollChartXRect.bottom + WORK_TRACE_MAGIN;
	calendarWtRect.right = calendarWtRect.left + 230;
	calendarWtRect.bottom = calendarWtRect.top + 200;
	calendarWt_.MoveWindow(&calendarWtRect, 1);

/*	rect.right = rect.left + chartRect.right - chartRect.left + WORK_TRACE_MAGIN * 2;
	rect.bottom = rect.top + chartRect.bottom - chartRect.top + WORK_TRACE_MAGIN * 2
		+ scrollChartXRect.bottom - scrollChartXRect.top + WORK_TRACE_MAGIN
		+ calendarWtRect.bottom - calendarWtRect.top;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
	MoveWindow(&rect, 1);
	CenterWindow();
*/
	Invalidate();
}

void WorkTraceDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	correctPos();
}

void WorkTraceDlg::OnDestroy()
{
	KillTimer(0);

	CDialogEx::OnDestroy();
}

void WorkTraceDlg::onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset)
{
	euhatChart_->startX_ = innerWindowOffset;
	::InvalidateRect(euhatChart_->hwnd_, NULL, 0);
}

void WorkTraceDlg::refreshChart()
{
	SYSTEMTIME t;
	calendarWt_.GetCurSel(&t);
	char filePath[256];
	sprintf(filePath, "%04d%02d%02d", t.wYear, t.wMonth, t.wDay);
	DBG(("open payload file [%s].\n", filePath));

	if (!euhatChart_->read(filePath)) {
//		AfxMessageBox(IDS_MSG_OPEN_PAYLOAD_FILE_FAILED);
	}
}

void WorkTraceDlg::OnSelchangeCalendarWt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMSELCHANGE pSelChange = reinterpret_cast<LPNMSELCHANGE>(pNMHDR);
	
	refreshChart();

	*pResult = 0;
}

void WorkTraceDlg::OnTimer(UINT_PTR nIDEvent)
{
	refreshChart();

	CDialogEx::OnTimer(nIDEvent);
}