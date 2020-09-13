#pragma once
#include "MfcBasePage.h"
#include <EuhatBitmap.h>
#include <EuhatButton.h>

class TitleBarDlg : public MfcBasePage, public EuhatButtonUser
{

	unique_ptr<EuhatButton> btnClose_;
	unique_ptr<EuhatButton> btnMax_;
	unique_ptr<EuhatButton> btnRestore_;
	unique_ptr<EuhatButton> btnMin_;
	unique_ptr<EuhatButton> btnSetting_;
	unique_ptr<EuhatButton> btnHelp_;
	unique_ptr<EuhatFont> font_;

	CRect rcRestore_;
	EuhatBitmap bmpBk_;

public:
	TitleBarDlg(EuhatBase *euhatBase);
	virtual ~TitleBarDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TITLE_BAR_DIALOG };
#endif

	virtual void DoDataExchange(CDataExchange *pDX);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnClickedBtnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnMax();
	afx_msg void OnBnClickedBtnMin();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnRestore();
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnBnClickedBtnHelp();
	void onBtnClick(EuhatButton *btn);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
