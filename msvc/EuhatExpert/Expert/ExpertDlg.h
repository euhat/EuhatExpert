#pragma once

#include <string>
#include <vector>
#include "MfcBasePage.h"

using namespace std;

#define EU_TIMER_ID_KQ_INFORM 1

#define EU_EXPERT_DLG_TITLE_HEIGHT 35
#define EU_EXPERT_DLG_LEFT_WIDTH 180
#define EU_EXPERT_TOOLBAR_HEIGHT 40
#define EU_EXPERT_DLG_MARGIN 8

#define EU_EXPERT_MAIN_DLG_MIN_WIDTH 800
#define EU_EXPERT_MAIN_DLG_MIN_HEIGHT 600

class KqInformDlg;
class EuhatLeftDlg;
class CfgDlg;
class WorkTrace;
class CExpertApp;

class CExpertDlg : public MfcBasePage
{
	unique_ptr<WorkTrace> workTrace_;

public:
	CExpertDlg(EuhatBase *euhatBase);
	virtual ~CExpertDlg();

	void correctPos();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPERT_DIALOG };
#endif

	HICON m_hIcon;
	CExpertApp *app_;
	EuhatBase *euhatBase_;

	unique_ptr<MfcBasePage> titleBar_;
	unique_ptr<EuhatLeftDlg> leftBar_;
	unique_ptr<KqInformDlg> kqInformDlg_;
	unique_ptr<CfgDlg> cfgDlg_;

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange *pDX);

	void exitNotifyIcon();

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	LRESULT OnUserExit(WPARAM wParam, LPARAM lParam);
	LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnMenuCmdExit();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void OnSizing(UINT fwSide, LPRECT pRect);
	int refreshUiLocalization();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
