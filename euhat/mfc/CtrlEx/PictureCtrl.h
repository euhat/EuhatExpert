///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////
// PictureCtrl.h
// 
// Author: Tobias Eiseler
//
// E-Mail: tobias.eiseler@sisternicky.com
// 
// Function: A MFC Picture Control to display
//           an image on a Dialog, etc.
///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

class CPictureCtrl :
	public CStatic
{
public:

	//Constructor
	CPictureCtrl(void);

	//Destructor
	~CPictureCtrl(void);

public:

	//Loads an image from a file
	BOOL LoadFromFile(LPCTSTR pszFilePath, LPRECT pShowRect = NULL,LPCRECT pPosRect = NULL, BOOL bErase = FALSE);

	//Loads an image from an IStream interface
	BOOL LoadFromStream(IStream* piStream, LPRECT pShowRect = NULL,LPCRECT pPosRect = NULL, BOOL bErase = FALSE);

	//Loads an image from a byte stream;
	BOOL LoadFromByte(BYTE* pData, size_t nSize, LPRECT pShowRect = NULL,LPCRECT pPosRect = NULL, BOOL bErase = FALSE);

	//Loads an image from a Resource
// 	BOOL LoadFromResource(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);

	//Overload - Single load function
	BOOL Load(LPCTSTR pszFilePath, LPRECT pShowRect = NULL,LPCRECT pPosRect = NULL, BOOL bErase = FALSE);
	BOOL Load(IStream* piStream, LPRECT pShowRect = NULL,LPCRECT pPosRect = NULL, BOOL bErase = FALSE);
	BOOL Load(BYTE* pData, size_t nSize, LPRECT pShowRect = NULL,LPCRECT pPosRect = NULL, BOOL bErase = FALSE);
// 	BOOL Load(HMODULE hModule, LPCTSTR lpName, LPCTSTR lpType);

	int GetPictureWidth();
	int GetPictureHeight();

	void SetShowRect(LPRECT pShowRect = NULL, BOOL bErase = FALSE);
	void SetPosRect(LPCRECT pPosRect = NULL, BOOL bErase = FALSE);
	void ShowPosition(BOOL bShow = TRUE, BOOL bErase = FALSE);
	void SetBkgText(LPCTSTR pszText, BOOL bErase = FALSE);
	void SetBkgText(LPCTSTR pszText, COLORREF bkColor, COLORREF txtColor, BOOL bErase = FALSE);

	//Frees the image data
	void FreeData(BOOL bErase = FALSE);

protected:
	virtual void PreSubclassWindow();

	//Draws the Control
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL OnEraseBkgnd(CDC* pDC);



private:

	//Internal image stream buffer
	IStream* m_pStream;

	//Control flag if a pic is loaded
	BOOL m_bIsPicLoaded;

	//GDI Plus Token
	ULONG_PTR m_gdiplusToken;

	//Show Rect
	CRect m_rcShow;

	// »­×ø±ê
	BOOL  m_bShowPos;
	CRect m_rcPosition;

	// ±³¾°
	CString  m_szBkgText;
	COLORREF m_clBkgColor;
	COLORREF m_clBkgTextColor;
};
