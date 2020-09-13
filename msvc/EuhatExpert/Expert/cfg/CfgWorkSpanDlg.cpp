#include "stdafx.h"
#include <common/OpCommon.h>
#include "../Expert.h"
#include "../ExpertDlg.h"
#include "CfgDlg.h"
#include "CfgWorkSpanDlg.h"
#include "afxdialogex.h"
#include <EuhatPostDefMfc.h>

CfgWorkSpanDlg::CfgWorkSpanDlg(EuhatBase *euhatBase, CWnd *parent)
	: MfcBasePage(IDD_CFG_WORK_SPAN_DIALOG, euhatBase, parent)
{

}

CfgWorkSpanDlg::~CfgWorkSpanDlg()
{
}

void CfgWorkSpanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START_TIME, dateTimeStartTime_);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END_TIME, dateTimeEndTime_);
}

BEGIN_MESSAGE_MAP(CfgWorkSpanDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CfgWorkSpanDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CfgWorkSpanDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	pushUiLoc("IS_NEGATIVE", "Input value is negative.");
	pushUiLoc("WARNING", "Warning");

	CString cstr;

	SetDlgItemInt(IDC_EDIT_INFORM_BEGIN_MINUTES, v_.beginSecs_ / 60);

	cstr = CA2T(v_.startTime_.c_str());
	COleDateTime oleTime;
	oleTime.ParseDateTime(cstr);
	dateTimeStartTime_.SetTime(oleTime);

	cstr = CA2T(v_.endTime_.c_str());
	oleTime.ParseDateTime(cstr);
	dateTimeEndTime_.SetTime(oleTime);

	SetDlgItemInt(IDC_EDIT_INFORM_UNTIL_MINUTES, v_.untilSecs_ / 60);

	string tag = euhatBase_->tagApp_ + "/" + euhatBase_->tagCfgDlg_ + "/" + euhatBase_->tagCfgWorkTraceDlg_ + "/" + "CfgWorkSpanDlg";
	euhatBase_->refreshUiLocalization(this, tag.c_str());

	return TRUE;
}

void CfgWorkSpanDlg::OnBnClickedOk()
{
	v_.beginSecs_ = GetDlgItemInt(IDC_EDIT_INFORM_BEGIN_MINUTES) * 60;
	v_.untilSecs_ = GetDlgItemInt(IDC_EDIT_INFORM_UNTIL_MINUTES) * 60;

	if (v_.beginSecs_ < 0 || v_.untilSecs_ < 0)
	{
		MessageBox(t("IS_NEGATIVE"), t("WARNING"), 0);
		return;
	}

	CString cstr;
	COleDateTime oleTime;
	dateTimeStartTime_.GetTime(oleTime);
	cstr = oleTime.Format(_T("%H:%M:00"));
	v_.startTime_ = CT2A(cstr);

	dateTimeEndTime_.GetTime(oleTime);
	cstr = oleTime.Format(_T("%H:%M:00"));
	v_.endTime_ = CT2A(cstr);

	CDialogEx::OnOK();
}
