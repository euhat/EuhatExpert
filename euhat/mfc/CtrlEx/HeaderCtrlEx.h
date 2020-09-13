#if !defined(AFX_HEADERCTRLEX_H__D3AF8460_912F_4AE5_A02F_7B8AD2A23265__INCLUDED_)
#define AFX_HEADERCTRLEX_H__D3AF8460_912F_4AE5_A02F_7B8AD2A23265__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeaderCtrlEx.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx window

class CHeaderCtrlEx : public CHeaderCtrl
{

	DECLARE_DYNAMIC(CHeaderCtrlEx)

public:
	CHeaderCtrlEx();
	virtual ~CHeaderCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	CStringArray m_HChar;
	CString m_Format; //表示对齐类型的整型数组,0表示左对齐，1表示中间对齐，2表示右对齐
public:
	int m_R;
	int m_G;
	int m_B;
	int m_Gradient;	// 画立体背景，渐变系数
	int m_Height;  //表头高度,
	int m_fontHeight; //字体高度
	int m_fontWidth;   //字体宽度
	COLORREF m_color;
	LRESULT OnLayout( WPARAM wParam, LPARAM lParam );
	
	int m_iFontOffset;
	COLORREF m_OffsetColor;
	void SetFontOffset(int iOffset, COLORREF OffsetColor);  //字体左侧偏移量, 默认为0

	void PaintWithBitmap(CDC * pDC,HBITMAP hBitmap);
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERCTRLEX_H__D3AF8460_912F_4AE5_A02F_7B8AD2A23265__INCLUDED_)
