#pragma once

class CNewSpinButton : public CSpinButtonCtrl
{
public:
	CNewSpinButton();

	HBITMAP m_hbmpDownBtn;
	BOOL m_bDown;
	BOOL m_bFirst;
	virtual ~CNewSpinButton();

protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
