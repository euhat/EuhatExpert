#pragma once

class CxSkinButton : public CButton
{
public:
	CxSkinButton();

public:
	int m_nSetBtnOrder;
	BOOL m_bMouseDown;
	
	BOOL m_OffState;

	CString m_strToolTip;

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	void SetToolTipText(CString s);
	COLORREF SetTextColor(COLORREF new_color);
	void SetSkin(UINT normal,UINT down, UINT over=NULL, 
		UINT disabled=NULL,UINT off = NULL,UINT mask=NULL,short drawmode=1,
		short border=1,UINT cursorID=0,short margin=4);
	void SetSkin(HBITMAP normal, HBITMAP down, HBITMAP over,HBITMAP disabled,
		         HBITMAP off, HBITMAP off_on = NULL, HBITMAP mask = NULL, short drawmode = 1,
				 short border = 0, UINT cursorID = 0, short margin = 0);

	void SetSkin(CBitmap *normal, CBitmap *down, CBitmap *over = NULL, 
		CBitmap *disabled = NULL, CBitmap *off = NULL, UINT mask = NULL, short drawmode = 1,
		short border = 1, UINT cursorID = 0, short margin = 4);
	void setButtonSkin(LPCTSTR folderName, LPCTSTR btnName);
	void SetOffState(BOOL);
	void SetOffSkin(UINT);
	void SetOffSkin(CBitmap *);
	virtual ~CxSkinButton();

protected:
	HCURSOR m_hCursor;
	BOOL SetBtnCursor(UINT cursorID);

protected:
    bool m_tracking;
    bool m_button_down;
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	CToolTipCtrl m_tooltip;
	CBitmap m_bmpNormal, m_bmpDown, m_bmpDisabled, m_bmpMask, m_bmpOver, m_bmpOff,m_bmpOffOn; //skin bitmaps
	short	m_FocusRectMargin;		//dotted margin offset
	COLORREF m_TextColor;			//button text color
	HRGN	m_hClipRgn;				//clipping region
	BOOL	m_bBorder;				//0=flat; 1=3D;
	short	m_DrawMode;				//0=normal; 1=stretch; 2=tiled;
	HRGN	CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);
	void	FillWithBitmap(CDC *dc, HBITMAP hbmp, RECT r);
	void	DrawBitmap(CDC *dc, HBITMAP hbmp, RECT r, int DrawMode);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	afx_msg BOOL OnClicked();
	DECLARE_MESSAGE_MAP()
};