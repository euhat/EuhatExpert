#pragma once

#include "../../MfcBasePage.h"
#include "WorkTraceChart.h"
#include <EuhatScrollHBar.h>

class WorkTraceChart;

class WorkTraceDlg : public MfcBasePage, public EuhatScrollBar::User
{
	void refreshChart();
	void correctPos();
	void onScrollBarChange(EuhatScrollBar *bar, int innerWindowOffset);

	unique_ptr<WorkTraceChart> euhatChart_;
	unique_ptr<EuhatScrollHBar> scrollChartX_;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()

public:
	WorkTraceDlg(EuhatBase *euhatBase);
	virtual ~WorkTraceDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WORK_TRACE_DIALOG };
#endif

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	CMonthCalCtrl calendarWt_;
	afx_msg void OnSelchangeCalendarWt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
