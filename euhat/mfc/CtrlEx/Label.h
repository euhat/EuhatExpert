#pragma once

#define	NM_LINKCLICK (WM_APP + 0x200)

class CLabel : public CStatic
{
public:

	enum LinkStyle {LinkNone, HyperLink, MailLink};
	enum FlashType {None, Text, Background};
	enum Type3D {Raised, Sunken};
	enum BackFillMode {Normal, Gradient};

	CLabel();
	virtual ~CLabel();
	virtual CLabel& SetBkColor(COLORREF crBkgnd, COLORREF crBkgndHigh = 0, BackFillMode mode = Normal);
	virtual CLabel& SetTextColor(COLORREF crText);
	virtual CLabel& SetText(LPCTSTR pstrText = NULL);
	virtual CLabel& GetTextExtent(CSize &cs);
	virtual CLabel& SetFontBold(BOOL bBold = TRUE);
	virtual CLabel& SetFontName(LPCTSTR pstrFont, BYTE byCharSet = ANSI_CHARSET);
	virtual CLabel& SetFontUnderline(BOOL bSet = TRUE);
	virtual CLabel& SetFontItalic(BOOL bSet = TRUE);
	virtual CLabel& SetFontSize(int nSize);
	virtual CLabel& SetSunken(BOOL bSet = TRUE);
	virtual CLabel& SetBorder(BOOL bSet = TRUE);
	virtual CLabel& SetTransparent(BOOL bSet = TRUE);
	virtual CLabel& FlashText(BOOL bActivate = TRUE);
	virtual CLabel& FlashBackground(BOOL bActivate = TRUE);
	virtual CLabel& SetLink(BOOL bLink, BOOL bNotifyParent = TRUE);
	virtual CLabel& SetLinkCursor(HCURSOR hCursor);
	virtual CLabel& SetFont3D(BOOL bSet = TRUE, Type3D type = Raised);
	virtual CLabel& SetRotationAngle(UINT nAngle, BOOL bRotation = TRUE);
	virtual CLabel& SetText3DHiliteColor(COLORREF cr3DHiliteColor);
	virtual CLabel& SetFont(LOGFONT lf);
	virtual CLabel& SetMailLink(BOOL bEnable = TRUE, BOOL bNotifyParent = TRUE);
	virtual CLabel& SetHyperLink(LPCTSTR psLink = NULL);
	virtual CLabel& SetVerticalCenter(BOOL bSet = TRUE);

protected:
	void UpdateSurface();
	void ReconstructFont();
	void DrawGradientFill(CDC *pDC, CRect *pRect, COLORREF crStart, COLORREF crEnd, int nSegments);
	COLORREF		m_crText;
	COLORREF		m_cr3DHiliteColor;
	HBRUSH			m_hwndBrush;
	HBRUSH			m_hBackBrush;
	LOGFONT			m_lf;
	CFont			m_font;
	BOOL			m_bState;
	BOOL			m_bTimer;
	LinkStyle		m_Link;
	BOOL			m_bTransparent;
	BOOL			m_bFont3d;
	BOOL			m_bToolTips;
	BOOL			m_bNotifyParent;
	BOOL			m_bRotation;
	FlashType		m_Type;
	HCURSOR			m_hCursor;
	Type3D			m_3dType;
	BackFillMode	m_fillmode;
	COLORREF		m_crHiColor;
	COLORREF		m_crLoColor;
	CString			m_sLink;
	BOOL			m_bVerticalCenter;

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg HBRUSH CtlColor(CDC *pDC, UINT nCtlColor);
};
