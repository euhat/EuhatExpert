#include "stdafx.h"
#include "Expert.h"
#include "ExpertDlg.h"
#include "afxdialogex.h"
#include "TitleBarDlg.h"
#include "EuhatLeftDlg.h"
#include "cfg/CfgDlg.h"
#include "KqInformDlg.h"
#include "local/WorkTrace/WorkTrace.h"
#include <OpWin.h>
#include <../config.h>
#include <EuhatPostDefMfc.h>

#define WM_USER_ICON_NOTIFY (WM_USER + 1)
#define WM_USER_EXIT		(WM_USER + 2)
#define WM_USER_UI_LOCALIZATION (WM_USER + 3)

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CExpertDlg::CExpertDlg(EuhatBase *euhatBase)
	: MfcBasePage(IDD_EXPERT_DIALOG, euhatBase)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	workTrace_ = NULL;

	euhatBase_ = euhatBase;
	euhatBase_->mainWnd_ = this;

	kqInformDlg_.reset(new KqInformDlg(euhatBase_));
	titleBar_.reset(new TitleBarDlg(euhatBase_));
	leftBar_.reset(new EuhatLeftDlg(euhatBase_));
	cfgDlg_.reset(new CfgDlg(euhatBase_));
	workTrace_.reset(new WorkTrace(euhatBase_));
}

CExpertDlg::~CExpertDlg()
{

}

void CExpertDlg::DoDataExchange(CDataExchange *pDX)
{
	MfcBasePage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExpertDlg, MfcBasePage)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER_ICON_NOTIFY, OnNotifyIcon)
	ON_MESSAGE(WM_USER_EXIT, OnUserExit)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENU_CMD_EXIT, &CExpertDlg::OnMenuCmdExit)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_NCHITTEST()
	ON_WM_QUERYENDSESSION()
END_MESSAGE_MAP()

BOOL CExpertDlg::OnInitDialog()
{
	MfcBasePage::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	pushUiLoc("SURE_TO_LEAVE", "Are you sure to leave?");
	pushUiLoc("MENU_EXIT", "Exit");

	SetWindowText(_T("Euhat Expert"));
	SetProp(m_hWnd, app_->m_pszExeName, (HANDLE)1);

	workTrace_->startRecord();

	CRect rect;
	GetClientRect(&rect);
	rect.bottom = rect.top + 30;

	titleBar_->create();
	titleBar_->ShowWindow(SW_SHOW);

	leftBar_->create();
	leftBar_->ShowWindow(SW_SHOW);

	cfgDlg_->create();
	cfgDlg_->CenterWindow();
	cfgDlg_->ShowWindow(SW_HIDE);

	euhatBase_->refreshUiLocalization();

	NOTIFYICONDATA iconData;
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = this->m_hWnd;
	iconData.uID = IDR_MAINFRAME;
	iconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	iconData.uCallbackMessage = WM_USER_ICON_NOTIFY;
	iconData.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CString csNotify;
	GetWindowText(csNotify);
	_tcscpy(iconData.szTip, csNotify);
	Shell_NotifyIcon(NIM_ADD, &iconData);

	correctPos();

	SetTimer(EU_TIMER_ID_KQ_INFORM, 15 * 1000, NULL);

	long style;
	style = GetWindowLong(m_hWnd, GWL_STYLE);
//	style &= ~WS_CHILD;
	style |= WS_MINIMIZEBOX;
	SetWindowLong(m_hWnd, GWL_STYLE, style);

	return TRUE;
}

int CExpertDlg::refreshUiLocalization()
{
	CString cstr;
	GetWindowText(cstr);
	cstr += _T(" v1.9.29.2");
	cstr += _T(" ");
	cstr += _T(BUILD_TAG);
	SetWindowText(cstr);

	titleBar_->Invalidate();

	return 1;
}

void CExpertDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		MfcBasePage::OnSysCommand(nID, lParam);
	}
}

void CExpertDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		CRect rc;
		GetClientRect(rc);

		CRect rcBottom = rc;
		rcBottom.top = rcBottom.bottom - EU_EXPERT_DLG_MARGIN;
		fillGradientRect(dc.m_hDC, rcBottom, RGB(140, 140, 255), RGB(140, 140, 255), FILL_STYLE_HORIZONTAL);

		CRect rcRight = rc;
		rcRight.left = rcRight.right - EU_EXPERT_DLG_MARGIN;
		fillGradientRect(dc.m_hDC, rcRight, RGB(90, 90, 200), RGB(140, 140, 255), FILL_STYLE_VERTICAL);

		CRect rcLeft = rc;
		rcLeft.right = rcLeft.left + EU_EXPERT_DLG_MARGIN;
		fillGradientRect(dc.m_hDC, rcLeft, RGB(90, 90, 200), RGB(140, 140, 255), FILL_STYLE_VERTICAL);

		CDialog::OnPaint();
	}
}

HCURSOR CExpertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CExpertDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	UINT uID;
	UINT uMouseMsg;
	POINT pt;
	uID = (UINT)wParam;
	uMouseMsg = (UINT)lParam;
	if (uMouseMsg == WM_LBUTTONDOWN) {
		switch (uID) {
		case IDR_MAINFRAME:
			//if (AfxGetApp()->m_pMainWnd->IsIconic())
			{
				BringWindowToTop();
				ShowWindow(SW_SHOWNORMAL);
			}
			break;
		}
	}
	if (uMouseMsg == WM_RBUTTONDOWN) {
		switch (uID) {
		case IDR_MAINFRAME: {
			CMenu menu;
			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MENU_ICON_NOTIFY);
			SetForegroundWindow();
			CMenu *pMenu;
			pMenu = menu.GetSubMenu(0);

			pMenu->ModifyMenu(ID_MENU_CMD_EXIT, MF_BYCOMMAND | MF_STRING, ID_MENU_CMD_EXIT, t("MENU_EXIT"));

			pMenu->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, pt.x, pt.y, this);
			HMENU hMenu = pMenu->Detach();
			pMenu->DestroyMenu();
			break;
		}
		default:
			break;
		}
	}
	return 0;
}

void CExpertDlg::exitNotifyIcon()
{
	NOTIFYICONDATA iconData;
	iconData.cbSize = sizeof(NOTIFYICONDATA);
	iconData.hWnd = this->m_hWnd;
	iconData.uID = IDR_MAINFRAME;
	Shell_NotifyIcon(NIM_DELETE, &iconData);
}

void CExpertDlg::correctPos()
{
	if (NULL == titleBar_->m_hWnd || NULL == leftBar_->m_hWnd)
		return;

	CRect rect;
	GetClientRect(&rect);

	int needMoveWin = 0;
	if (rect.Width() < EU_EXPERT_MAIN_DLG_MIN_WIDTH)
	{
		rect.right = EU_EXPERT_MAIN_DLG_MIN_WIDTH + rect.left;
		needMoveWin = 1;
	}
	if (rect.Height() < EU_EXPERT_MAIN_DLG_MIN_HEIGHT)
	{
		rect.bottom = EU_EXPERT_MAIN_DLG_MIN_HEIGHT - rect.top;
		needMoveWin = 1;
	}
	if (needMoveWin)
	{
		CRect rectWin = rect;
		ClientToScreen(&rectWin);
		MoveWindow(rectWin);
	}
	
	CRect rectLeft = rect;
	rectLeft.left += EU_EXPERT_DLG_MARGIN;
	rectLeft.bottom -= EU_EXPERT_DLG_MARGIN;
	rectLeft.right -= EU_EXPERT_DLG_MARGIN;
	leftBar_->correctPos(rectLeft);

	CRect rectTitle = rect;
	rectTitle.bottom = rectTitle.top + EU_EXPERT_DLG_TITLE_HEIGHT + EU_EXPERT_DLG_MARGIN;
	titleBar_->MoveWindow(&rectTitle);

#if 0
#define  WND_ROUND_ANGLE_SPACE 15
	CRect rcRound;
	GetWindowRect(&rcRound);
	rcRound -= rcRound.TopLeft();

	CRgn rgnTitle;
	rgnTitle.CreateRoundRectRgn(rcRound.left, rcRound.top, rcRound.right + 1, 200, WND_ROUND_ANGLE_SPACE, WND_ROUND_ANGLE_SPACE);

	CRgn rgnMain;
	rgnMain.CreateRectRgn(rcRound.left, rcRound.top + 100, rcRound.right, rcRound.bottom);

	CRgn rgn;
	rgn.CreateRectRgn(0, 0, 50, 50);
	rgn.CombineRgn(&rgnTitle, &rgnMain, RGN_OR);

	SetWindowRgn(rgn, TRUE);

	rgn.DeleteObject();
#endif
}

void CExpertDlg::OnMenuCmdExit()
{
	PostMessage(WM_USER_EXIT);
}

LRESULT CExpertDlg::OnUserExit(WPARAM wParam, LPARAM lParam)
{
	ShowWindow(SW_SHOW);

	CString title;
	GetWindowText(title);

	if (IDOK != MessageBox(t("SURE_TO_LEAVE"), title, 1)) {
		return 0;
	}

	exitNotifyIcon();

	PostMessage(WM_CLOSE);

	return 1;
}

void CExpertDlg::OnClose()
{
	KillTimer(EU_TIMER_ID_KQ_INFORM);

	kqInformDlg_->close();

	/*
	 * here, we must call leftBar_->DestroyWindow first, because if we call CExpertDlg::DestroyWindow first,
	 * some global resources will be destroyed first, then some objects like IPADDRESS UiControl called in
	 * leftBar_->DestroyWindow will visit these global resources, so it will produce "Critical error detected".
	 * 
	 */
	leftBar_->DestroyWindow();

	CDialog::OnClose();

	DestroyWindow();
}

void CExpertDlg::OnDestroy()
{
	MfcBasePage::OnDestroy();

	workTrace_->stopRecord();

	PostQuitMessage(0);
}

void CExpertDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (EU_TIMER_ID_KQ_INFORM == nIDEvent) {
		
		kqInformDlg_->check();
		
		if (NULL != kqInformDlg_->m_hWnd) {
			::SetWindowPos(kqInformDlg_->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	MfcBasePage::OnTimer(nIDEvent);
}

BOOL CExpertDlg::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;

	return MfcBasePage::OnEraseBkgnd(pDC);
}

void CExpertDlg::OnSize(UINT nType, int cx, int cy)
{
	MfcBasePage::OnSize(nType, cx, cy);

	correctPos();
}

void CExpertDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	int minWidth = EU_EXPERT_MAIN_DLG_MIN_WIDTH;
	int minHeight = EU_EXPERT_MAIN_DLG_MIN_HEIGHT;

	if (pRect->right - pRect->left < minWidth)
	{
		if (fwSide == WMSZ_LEFT
			|| fwSide == WMSZ_TOPLEFT
			|| fwSide == WMSZ_BOTTOMLEFT)
		{
			pRect->left = pRect->right - minWidth;
		}
		else
			pRect->right = pRect->left + minWidth;
	}
	if (pRect->bottom - pRect->top < minHeight)
	{
		if (fwSide == WMSZ_TOP
			|| fwSide == WMSZ_TOPLEFT
			|| fwSide == WMSZ_TOPRIGHT)
		{
			pRect->top = pRect->bottom - minHeight;
		}
		else
			pRect->bottom = pRect->top + minHeight;
	}
	Invalidate(TRUE);

	MfcBasePage::OnSizing(fwSide, pRect);
}

LRESULT CExpertDlg::OnNcHitTest(CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);

	if (pt.x < rc.left + 20 && pt.y < rc.top + 20)
	{
		return HTTOPLEFT;
	}
	else if (pt.x > rc.right - 20 && pt.y < rc.top + 20)
	{
		return HTTOPRIGHT;
	}
	else if (pt.x < rc.left + 20 && pt.y > rc.bottom - 20)
	{
		return HTBOTTOMLEFT;
	}
	else if (pt.x > rc.right - 20 && pt.y > rc.bottom - 20)
	{
		return HTBOTTOMRIGHT;
	}
	else if (pt.x < rc.left + 20)
	{
		return HTLEFT;
	}
	else if (pt.x > rc.right - 20)
	{
		return HTRIGHT;
	}
	else if (pt.y < rc.top + 20)
	{
		return HTTOP;
	}
	else if (pt.y > rc.bottom - 20)
	{
		return HTBOTTOM;
	}

	return MfcBasePage::OnNcHitTest(pt);
}

LRESULT CExpertDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_ENDSESSION || message == WM_QUERYENDSESSION)
	{
		PostMessage(WM_CLOSE);
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}