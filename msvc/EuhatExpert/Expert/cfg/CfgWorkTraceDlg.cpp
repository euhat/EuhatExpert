#include "stdafx.h"
#include "../Expert.h"
#include "CfgDlg.h"
#include "CfgWorkTraceDlg.h"
#include "CfgWorkSpanDlg.h"
#include "../local/WorkTrace/WorkTraceDlg.h"
#include "afxdialogex.h"
#include <EuhatPostDefMfc.h>

CfgWorkTraceDlg::CfgWorkTraceDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_CFG_WORK_TRACE_DIALOG, euhatBase)
{

}

CfgWorkTraceDlg::~CfgWorkTraceDlg()
{

}

void CfgWorkTraceDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ATTENDANCE_REGULATION, listAR_);
}

BEGIN_MESSAGE_MAP(CfgWorkTraceDlg, CDialogEx)
	ON_BN_CLICKED(IDC_KQ_TIME_SPAN_ADD, &CfgWorkTraceDlg::OnBnClickedKqTimeSpanAdd)
	ON_BN_CLICKED(IDC_KQ_TIME_SPAN_DEL, &CfgWorkTraceDlg::OnBnClickedKqTimeSpanDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ATTENDANCE_REGULATION, &CfgWorkTraceDlg::OnDblclkListAttendanceRegulation)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ATTENDANCE_REGULATION, &CfgWorkTraceDlg::OnClickListAttendanceRegulation)
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

BOOL CfgWorkTraceDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();
	pushUiLoc("BEGIN_MINUTES", "Inform Begin Minutes");
	pushUiLoc("START_TIME", "Start Time");
	pushUiLoc("END_TIME", "End Time");
	pushUiLoc("UNTIL_MINUTES", "Inform UNTIL Minutes");

	listAR_.SetExtendedStyle(listAR_.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	int colId = 0;
	listAR_.InsertColumn(colId++, t("BEGIN_MINUTES"), LVCFMT_CENTER, 150);
	listAR_.InsertColumn(colId++, t("START_TIME"), LVCFMT_CENTER, 150);
	listAR_.InsertColumn(colId++, t("END_TIME"), LVCFMT_CENTER, 150);
	listAR_.InsertColumn(colId++, t("UNTIL_MINUTES"), LVCFMT_CENTER, 150);

	int lastIdx = 0;

	for (JyList<WorkSpan>::iterator it = euhatBase_->workSpans_.list_.begin(); it != euhatBase_->workSpans_.list_.end(); it++)
	{
		int colId = 0;
		CString cstr = CA2T(intToString(lastIdx).c_str());
		listAR_.InsertItem(lastIdx, cstr);

		cstr.Format(_T("%d"), it->beginSecs_ / 60);
		listAR_.SetItemText(lastIdx, colId++, cstr);
		cstr = CA2T(it->startTime_.c_str());
		listAR_.SetItemText(lastIdx, colId++, cstr);
		cstr = CA2T(it->endTime_.c_str());
		listAR_.SetItemText(lastIdx, colId++, cstr);
		cstr.Format(_T("%d"), it->untilSecs_ / 60);
		listAR_.SetItemText(lastIdx++, colId++, cstr);
	}

	return TRUE;
}

int CfgWorkTraceDlg::refreshUiLocalization()
{
	CString beginMin = t("BEGIN_MINUTES");
	CString startTime = t("START_TIME");
	CString endTime = t("END_TIME");
	CString untilMin = t("UNTIL_MINUTES");

	TCHAR buf[1024];
	buf[1023] = 0;
	LVCOLUMN col;
	memset(&col, 0, sizeof(col));
	col.mask = col.mask | LVCF_WIDTH | LVCF_FMT | LVCF_TEXT;

	col.pszText = buf;
	col.cchTextMax = 1023;

	listAR_.GetColumn(0, &col);
	col.pszText = (LPWSTR)(LPCWSTR)beginMin;
	listAR_.SetColumn(0, &col);

	col.pszText = buf;
	col.cchTextMax = 1023;

	listAR_.GetColumn(1, &col);
	col.pszText = (LPWSTR)(LPCWSTR)startTime;
	listAR_.SetColumn(1, &col);

	col.pszText = buf;
	col.cchTextMax = 1023;

	listAR_.GetColumn(2, &col);
	col.pszText = (LPWSTR)(LPCWSTR)endTime;
	listAR_.SetColumn(2, &col);

	col.pszText = buf;
	col.cchTextMax = 1023;

	listAR_.GetColumn(3, &col);
	col.pszText = (LPWSTR)(LPCWSTR)untilMin;
	listAR_.SetColumn(3, &col);

	return 1;
}

void CfgWorkTraceDlg::OnBnClickedKqTimeSpanAdd()
{
	CfgWorkSpanDlg dlg(euhatBase_, this);
	dlg.v_.beginSecs_ = 30 * 60;
	dlg.v_.startTime_ = "08:30:00";
	dlg.v_.endTime_ = "17:30:00";
	dlg.v_.untilSecs_ = 2 * 60 * 60;
	if (IDOK == dlg.DoModal())
	{
		int lastIdx = listAR_.GetItemCount();

		int colId = 0;
		CString cstr = CA2T(intToString(lastIdx).c_str());
		listAR_.InsertItem(lastIdx, cstr);

		cstr.Format(_T("%d"), dlg.v_.beginSecs_ / 60);
		listAR_.SetItemText(lastIdx, colId++, cstr);
		cstr = CA2T(dlg.v_.startTime_.c_str());
		listAR_.SetItemText(lastIdx, colId++, cstr);
		cstr = CA2T(dlg.v_.endTime_.c_str());
		listAR_.SetItemText(lastIdx, colId++, cstr);
		cstr.Format(_T("%d"), dlg.v_.untilSecs_ / 60);
		listAR_.SetItemText(lastIdx, colId++, cstr);

		save();
	}
}

void CfgWorkTraceDlg::OnBnClickedKqTimeSpanDel()
{
	int selected = listAR_.GetNextItem(-1, LVIS_SELECTED);
	if (selected < 0)
	{
		return;
	}
	listAR_.DeleteItem(selected);

	save();
}

void CfgWorkTraceDlg::OnDblclkListAttendanceRegulation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int selected = listAR_.GetNextItem(-1, LVIS_SELECTED);
	if (selected < 0)
	{
		return;
	}

	CfgWorkSpanDlg dlg(euhatBase_, this);
	int colId = 0;
	CString cstr;
	cstr = listAR_.GetItemText(selected, colId++);
	dlg.v_.beginSecs_ = atoi(CT2A(cstr)) * 60;
	cstr = listAR_.GetItemText(selected, colId++);
	dlg.v_.startTime_ = CT2A(cstr);
	cstr = listAR_.GetItemText(selected, colId++);
	dlg.v_.endTime_ = CT2A(cstr);
	cstr = listAR_.GetItemText(selected, colId++);
	dlg.v_.untilSecs_ = atoi(CT2A(cstr)) * 60;
	if (IDOK == dlg.DoModal())
	{
		colId = 0;
		cstr.Format(_T("%d"), dlg.v_.beginSecs_ / 60);
		listAR_.SetItemText(selected, colId++, cstr);
		cstr = CA2T(dlg.v_.startTime_.c_str());
		listAR_.SetItemText(selected, colId++, cstr);
		cstr = CA2T(dlg.v_.endTime_.c_str());
		listAR_.SetItemText(selected, colId++, cstr);
		cstr.Format(_T("%d"), dlg.v_.untilSecs_ / 60);
		listAR_.SetItemText(selected, colId++, cstr);

		save();
	}

	*pResult = 0;
}

int CfgWorkTraceDlg::save()
{
	euhatBase_->workSpans_.list_.clear();

	int count = listAR_.GetItemCount();
	for (int i = 0; i < count; i++)
	{
		WorkSpan ws;
		int colId = 0;
		CString cstr;
		cstr = listAR_.GetItemText(i, colId++);
		ws.beginSecs_ = atoi(CT2A(cstr)) * 60;
		cstr = listAR_.GetItemText(i, colId++);
		ws.startTime_ = CT2A(cstr);
		cstr = listAR_.GetItemText(i, colId++);
		ws.endTime_ = CT2A(cstr);
		cstr = listAR_.GetItemText(i, colId++);
		ws.untilSecs_ = atoi(CT2A(cstr)) * 60;

		euhatBase_->workSpans_.list_.push_back(ws);
	}

	euhatBase_->save();

	return 1;
}

void CfgWorkTraceDlg::OnClickListAttendanceRegulation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	*pResult = 0;
}
