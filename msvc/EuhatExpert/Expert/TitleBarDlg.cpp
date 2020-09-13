#include "stdafx.h"
#include "Expert.h"
#include "TitleBarDlg.h"
#include "afxdialogex.h"
#include "ExpertDlg.h"
#include "cfg/CfgDlg.h"
#include <OpWin.h>
#include <EuhatPostDefMfc.h>

TitleBarDlg::TitleBarDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_TITLE_BAR_DIALOG, euhatBase)
{
	font_.reset(new EuhatFont(_T("MS Shell Dlg"), 40));
}

TitleBarDlg::~TitleBarDlg()
{
}

void TitleBarDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TitleBarDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &TitleBarDlg::OnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_MAX, &TitleBarDlg::OnBnClickedBtnMax)
	ON_BN_CLICKED(IDC_BTN_MIN, &TitleBarDlg::OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_RESTORE, &TitleBarDlg::OnBnClickedBtnRestore)
	ON_BN_CLICKED(IDC_BTN_SETTING, &TitleBarDlg::OnBnClickedBtnSetting)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_HELP, &TitleBarDlg::OnBnClickedBtnHelp)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL TitleBarDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	char path[1024];
	sprintf(path, "%s/%s/%s_%s.jpg", EUHAT_BUTTON_SKIN_PATH, "titleBar", "N", "02");
	bmpBk_.init(path);
	euhatBase_->bmpBk_ = &bmpBk_;

	btnClose_.reset(new EuhatButton(m_hWnd, this)); btnClose_->setSkin("titleBar", "07");
	btnMax_.reset(new EuhatButton(m_hWnd, this)); btnMax_->setSkin("titleBar", "06");
	btnRestore_.reset(new EuhatButton(m_hWnd, this)); btnRestore_->setSkin("titleBar", "11");
	btnHelp_.reset(new EuhatButton(m_hWnd, this)); btnHelp_->setSkin("titleBar", "10");
	btnMin_.reset(new EuhatButton(m_hWnd, this)); btnMin_->setSkin("titleBar", "04");
	btnSetting_.reset(new EuhatButton(m_hWnd, this)); btnSetting_->setSkin("titleBar", "05");

	::ShowWindow(btnRestore_->hwnd_, SW_HIDE);

	return TRUE;
}

void TitleBarDlg::OnClickedBtnClose()
{
	GetParent()->PostMessage(WM_COMMAND, ID_MENU_CMD_EXIT, 0);
}

void TitleBarDlg::OnBnClickedBtnMax()
{
	CExpertDlg *mainDlg = (CExpertDlg *)euhatBase_->mainWnd_;
	mainDlg->GetWindowRect(&rcRestore_);

	CRect rcWA;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWA, 0);
	mainDlg->MoveWindow(&rcWA);

	::ShowWindow(btnMax_->hwnd_, SW_HIDE);
	::ShowWindow(btnRestore_->hwnd_, SW_SHOW);
}

void TitleBarDlg::OnBnClickedBtnRestore()
{
	CExpertDlg *mainDlg = (CExpertDlg *)euhatBase_->mainWnd_;
	mainDlg->MoveWindow(&rcRestore_);

	::ShowWindow(btnMax_->hwnd_, SW_SHOW);
	::ShowWindow(btnRestore_->hwnd_, SW_HIDE);
}

void TitleBarDlg::OnBnClickedBtnMin()
{
	//euhatBase_->mainWnd_->PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	euhatBase_->mainWnd_->ShowWindow(SW_HIDE);
}

void TitleBarDlg::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;
	GetClientRect(rc);
	int iWidth = rc.Width();
	int iHeight = rc.Height();
	
/*	::StretchBlt(dc.m_hDC, 0, 0, iWidth, bmpBk_.getHeight(), bmpBk_.hdc_,
		0, 0, bmpBk_.getWidth(), bmpBk_.getHeight(), SRCCOPY);
*/

	CRect rcBottom = rc;
	fillGradientRect(dc.m_hDC, rcBottom, RGB(140, 140, 255), RGB(100, 100, 200), FILL_STYLE_VERTICAL);

	CString cstr;
	euhatBase_->mainWnd_->GetWindowText(cstr);
	SetTextColor(dc.m_hDC, RGB(255, 255, 255));
	SetBkMode(dc.m_hDC, TRANSPARENT);

	EuhatGuard<EuhatFont> fontGuard(font_.get(), dc.m_hDC);
	TextOut(dc.m_hDC, 8, 6, cstr, cstr.GetLength());
}

void TitleBarDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	if (btnClose_.get() == NULL)
		return;

	CRect rect;
	GetClientRect(&rect);

	CRect rectClose;
	::GetClientRect(btnClose_->hwnd_, &rectClose);
	rectClose.MoveToXY(rect.right - rectClose.Width() - 10, 0);
	btnClose_->moveWindow(&rectClose);

	CRect rectMax;
	::GetClientRect(btnMax_->hwnd_, &rectMax);
	rectMax.MoveToXY(rectClose.left - rectMax.Width(), 0);
	btnMax_->moveWindow(&rectMax);

	btnRestore_->moveWindow(&rectMax);

	CRect rectHelp;
	::GetClientRect(btnHelp_->hwnd_, &rectHelp);
	rectHelp.MoveToXY(rectMax.left - rectHelp.Width(), 0);
	btnHelp_->moveWindow(&rectHelp);

	CRect rectSetting;
	::GetClientRect(btnSetting_->hwnd_, &rectSetting);
	rectSetting.MoveToXY(rectHelp.left - rectSetting.Width(), 0);
	btnSetting_->moveWindow(&rectSetting);

	CRect rectMin;
	::GetClientRect(btnMin_->hwnd_, &rectMin);
	rectMin.MoveToXY(rectSetting.left - rectMin.Width(), 0);
	btnMin_->moveWindow(&rectMin);

	Invalidate();
}

void TitleBarDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!::IsWindowVisible(btnMax_->hwnd_))
		return;

	GetParent()->SendMessage(WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

	MfcBasePage::OnLButtonDown(nFlags, point);
}

void TitleBarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	MfcBasePage::OnLButtonUp(nFlags, point);
}

void TitleBarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	MfcBasePage::OnMouseMove(nFlags, point);
}

void TitleBarDlg::OnBnClickedBtnSetting()
{
	euhatBase_->mainWnd_->cfgDlg_->ShowWindow(SW_SHOW);
}

void TitleBarDlg::OnBnClickedBtnHelp()
{
	TCHAR *url;
#ifdef EUHAT_EDITION_WORK_TRACE
	if (euhatBase_->curUiLanguage_ == EUHAT_UI_LANGUAGE_CN_SIMPLIFIED)
		url = _T("http://euhat.com/worktrace_cn.html");
	else
		url = _T("http://euhat.com/worktrace.html");
#else
	if (euhatBase_->curUiLanguage_ == EUHAT_UI_LANGUAGE_CN_SIMPLIFIED)
		url = _T("http://euhat.com/expert/index_cn.php");
	else
		url = _T("http://euhat.com/expert/");
#endif
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW);
}

void TitleBarDlg::onBtnClick(EuhatButton *btn)
{
	if (btn == btnClose_.get())
		OnClickedBtnClose();
	else if (btn == btnMax_.get())
		OnBnClickedBtnMax();
	else if (btn == btnRestore_.get())
		OnBnClickedBtnRestore();
	else if (btn == btnMin_.get())
		OnBnClickedBtnMin();
	else if (btn == btnSetting_.get())
		OnBnClickedBtnSetting();
	else if (btn == btnHelp_.get())
		OnBnClickedBtnHelp();
}

BOOL TitleBarDlg::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;

	return MfcBasePage::OnEraseBkgnd(pDC);
}

void TitleBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (::IsWindowVisible(btnMax_->hwnd_))
		OnBnClickedBtnMax();
	else
		OnBnClickedBtnRestore();
	__super::OnLButtonDblClk(nFlags, point);
}
