///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////
// PictureCtrl.cpp
// 
// Author: Tobias Eiseler
//
// E-Mail: tobias.eiseler@sisternicky.com
// 
// Function: A MFC Picture Control to display
//           an image on a Dialog, etc.
///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "PictureCtrl.h"
#include <GdiPlus.h>
using namespace Gdiplus;

//Macro to release COM Components

#ifdef SAFE_RELEASE
#undef SAFE_RELEASE
#endif
#define SAFE_RELEASE(x) do{\
							if((x) != NULL)\
							{\
								while((x)->Release() != 0);\
								(x) = NULL;\
							}\
						}while(0)

CPictureCtrl::CPictureCtrl(void)
	:CStatic()
	, m_pStream(NULL)
	, m_bIsPicLoaded(FALSE)
	, m_gdiplusToken(0)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_rcShow.SetRectEmpty();
	m_bShowPos = FALSE;
	m_rcPosition.SetRectEmpty();
	m_clBkgColor     = RGB(0,0,0);
	m_clBkgTextColor = RGB(255,255,127);
	m_szBkgText.Empty();
}

CPictureCtrl::~CPictureCtrl(void)
{
	//Tidy up
	FreeData();
	GdiplusShutdown(m_gdiplusToken);
}

BOOL CPictureCtrl::LoadFromStream(IStream *piStream, LPRECT pShowRect,LPCRECT pPosRect, BOOL bErase)
{
	//Set success error state
	SetLastError(ERROR_SUCCESS);

	FreeData();

	if (pShowRect)
	{
		m_rcShow = pShowRect;
	}
	else
	{
		m_rcShow.SetRectEmpty();
	}

	if (pPosRect)
	{
		m_rcPosition = pPosRect;
	}
	else
	{
		m_rcPosition.SetRectEmpty();
	}

	//Check for validity of argument
	if(piStream == NULL)
	{
		SetLastError(ERROR_INVALID_ADDRESS);
		return FALSE;
	}

	//Allocate stream
	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK)
	{
		SetLastError(dwResult);
		return FALSE;
	}

	//Rewind the argument stream
	LARGE_INTEGER lInt;
	lInt.QuadPart = 0;
	piStream->Seek(lInt, STREAM_SEEK_SET, NULL);

	//Read the lenght of the argument stream
	STATSTG statSTG;
	dwResult = piStream->Stat(&statSTG, STATFLAG_DEFAULT);
	if(dwResult != S_OK)
	{
		SetLastError(dwResult);
		SAFE_RELEASE(m_pStream);
		return FALSE;
	}

	//Copy the argument stream to the class stream
	piStream->CopyTo(m_pStream, statSTG.cbSize, NULL, NULL);

	//Mark as loaded
	m_bIsPicLoaded = TRUE;

	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}

	return TRUE;
}

BOOL CPictureCtrl::LoadFromByte(BYTE* pData, size_t nSize, LPRECT pShowRect, LPCRECT pPosRect, BOOL bErase)
{
	//Set success error state
	SetLastError(ERROR_SUCCESS);
	FreeData();

	if (pShowRect)
	{
		m_rcShow = pShowRect;
	}
	else
	{
		m_rcShow.SetRectEmpty();
	}

	if (pPosRect)
	{
		m_rcPosition = pPosRect;
	}
	else
	{
		m_rcPosition.SetRectEmpty();
	}

	//Allocate stream
	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK)
	{
		SetLastError(dwResult);
		return FALSE;
	}

	//Copy argument data to the stream
	dwResult = m_pStream->Write(pData, (ULONG)nSize, NULL);
	if(dwResult != S_OK)
	{
		SetLastError(dwResult);
		SAFE_RELEASE(m_pStream);
		return FALSE;
	}

	//Mark as loaded
	m_bIsPicLoaded = TRUE;

	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}

	return TRUE;
}

BOOL CPictureCtrl::LoadFromFile(LPCTSTR pszFilePath, LPRECT pShowRect,LPCRECT pPosRect, BOOL bErase)
{
	//Set success error state
	SetLastError(ERROR_SUCCESS);
	FreeData();

	if (pShowRect)
	{
		m_rcShow = pShowRect;
	}
	else
	{
		m_rcShow.SetRectEmpty();
	}

	if (pPosRect)
	{
		m_rcPosition = pPosRect;
	}
	else
	{
		m_rcPosition.SetRectEmpty();
	}

	//Allocate stream
	DWORD dwResult = CreateStreamOnHGlobal(NULL, TRUE, &m_pStream);
	if(dwResult != S_OK)
	{
		SetLastError(dwResult);
		return FALSE;
	}

	//Open the specified file
	CFile cFile;
	CFileException cFileException;
	if(!cFile.Open(pszFilePath, CStdioFile::modeRead | CStdioFile::typeBinary, &cFileException))
	{
		SetLastError(cFileException.m_lOsError);
		SAFE_RELEASE(m_pStream);
		return FALSE;
	}

	//Copy the specified file's content to the stream
	BYTE pBuffer[1024] = {0};
	while(UINT dwRead = cFile.Read(pBuffer, 1024))
	{
		dwResult = m_pStream->Write(pBuffer, dwRead, NULL);
		if(dwResult != S_OK)
		{
			SetLastError(dwResult);
			SAFE_RELEASE(m_pStream);
			cFile.Close();
			return FALSE;
		}
	}

	//Close the file
	cFile.Close();

	//Mark as Loaded
	m_bIsPicLoaded = TRUE;

	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}

	return TRUE;
}

//Overload - Single load function
BOOL CPictureCtrl::Load(LPCTSTR pszFilePath, LPRECT pShowRect, LPCRECT pPosRect, BOOL bErase)
{
	return LoadFromFile(pszFilePath, pShowRect, pPosRect, bErase);
}

BOOL CPictureCtrl::Load(IStream* piStream, LPRECT pShowRect, LPCRECT pPosRect, BOOL bErase)
{
	return LoadFromStream(piStream, pShowRect, pPosRect, bErase);
}

BOOL CPictureCtrl::Load(BYTE* pData, size_t nSize, LPRECT pShowRect, LPCRECT pPosRect, BOOL bErase)
{
	return LoadFromByte(pData, nSize, pShowRect, pPosRect, bErase);
}

int CPictureCtrl::GetPictureWidth()
{
	if (m_pStream)
	{
		Image image(m_pStream);
		return image.GetWidth();
	}
	return 0;
}

int CPictureCtrl::GetPictureHeight()
{
	if (m_pStream)
	{
		Image image(m_pStream);
		return image.GetHeight();
	}
	return 0;
}

void CPictureCtrl::SetShowRect(LPRECT pShowRect, BOOL bErase)
{
	if (pShowRect)
	{
		m_rcShow = pShowRect;
	}
	else
	{
		m_rcShow.SetRectEmpty();
	}

	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}
}

void CPictureCtrl::SetPosRect(LPCRECT pPosRect, BOOL bErase)
{
	if (pPosRect)
	{
		m_rcPosition = pPosRect;
	}
	else
	{
		m_rcPosition.SetRectEmpty();
	}

	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}
}

void CPictureCtrl::ShowPosition(BOOL bShow, BOOL bErase)
{
	if (m_bShowPos != bShow)
	{
		m_bShowPos = bShow;
	
		if (bErase)
		{
			Invalidate();
			RedrawWindow();
		}
	}
}

void CPictureCtrl::FreeData(BOOL bErase)
{
	m_bIsPicLoaded = FALSE;
	SAFE_RELEASE(m_pStream);
	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}

}

void CPictureCtrl::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();
	ModifyStyle(0, SS_OWNERDRAW);
}

void CPictureCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//Get control measures
	RECT rc;
	this->GetClientRect(&rc);

	Graphics graphics(lpDrawItemStruct->hDC);
	Image image(m_pStream);

	int w = rc.right-rc.left;
	int h = rc.bottom - rc.top;
	Rect destRect(rc.left, rc.top, w, h);

	//Check if pic data is loaded
	if(m_bIsPicLoaded)
	{
		if (m_rcShow.IsRectEmpty())
		{
			graphics.DrawImage(&image, destRect);
		}
		else
		{
			graphics.DrawImage(&image, destRect, m_rcShow.left, m_rcShow.top, m_rcShow.Width(), m_rcShow.Height(), Gdiplus::UnitPixel);
		}

		// 画坐标框
		if (m_bShowPos && !m_rcPosition.IsRectEmpty())
		{
			int x1 = (int)(m_rcPosition.left * 1.0 * w / image.GetWidth());
			int y1 = (int)(m_rcPosition.top * 1.0 * h / image.GetHeight());
			int x2 = (int)(m_rcPosition.right * 1.0 * w / image.GetWidth());
			int y2 = (int)(m_rcPosition.bottom * 1.0 * h / image.GetHeight());

			Pen redpen(Color::Red, 2.0f);
			Point pts[5] = {Point(x1, y1),Point(x1, y2),Point(x2, y2),Point(x2, y1),Point(x1, y1)};
			graphics.DrawLines(&redpen,pts,5);
		}
	}
	else
	{
		Color bkColor;
		bkColor.SetFromCOLORREF(m_clBkgColor);
		SolidBrush grayBrush(bkColor);
		graphics.FillRectangle(&grayBrush,destRect);
		if (!m_szBkgText.IsEmpty())
		{
			//DrawString( const WCHAR *string, INT length, const Font *font, const PointF &origin, const Brush *brush ){...}
			//graphics.DrawString(CT2W(m_szBkgText),m_szBkgText.GetLength,)
			//DrawTextInRect(m_pMemDC, rcClient, m_szBkgText, m_clBkgTextColor);
			FontFamily fontFamily(L"微软雅黑");   
			//---------------------------------------  
			// font四个参数，第二个：字体高度，单位由第四个决定  
			// 第三个：字体风格: FontStyleRegular(正常)、FontStyleBold(加粗)、 FontStyleItalic(斜体)、  
			//  FontStyleBoldItalic (粗斜体)、 FontStyleUnderline(下划线)  FontStyleStrikeout(删除线)  
			// 第四个:UnitDisplay(1/75英寸) UnitPixel(像素) UnitPoint(点、一点约折合为1/72英寸) UnitInch(英寸) UnitDocument(1/300英寸) UnitMillimeter(毫米)  
			//---------------------------------------  
			REAL fTxtHeight = 36;
			fTxtHeight = min(fTxtHeight, ((REAL)(w*2/3)));
			fTxtHeight = min(fTxtHeight, ((REAL)(h*2/3)));
			Gdiplus::Font font(&fontFamily, fTxtHeight, FontStyleRegular, UnitPixel);  

			StringFormat stringformat;  
			//-----------------------------------------  
			//  StringAlignmentNear(左对齐或右对齐，取决于书写方向是从左到右还是从右到左)、  
			//  StringAlignmentCenter(水平居中)  
			//  StringAlignmentFar(两端对齐)。  
			//  默认都不影响垂直  
			//-----------------------------------------  
			stringformat.SetAlignment(StringAlignmentCenter);  

			//-----------------------------------------  
			// 和SetAlignment对应，为竖直的设置  
			//-----------------------------------------  
			stringformat.SetLineAlignment(StringAlignmentCenter);  

			//------------------------------------------  
			// SolidBrush  
			// HatchBrush(阴影画刷)  
			// TextureBrush(纹理画刷):使用图像来填充形状的内部。无法继承此类  
			// LinearGradientBrush(渐变画刷)  
			// 注意，用的是Color初始化，不要用RGB初始化，不会报错，但绘不出来！  
			//------------------------------------------  
			Color txtColor;
			txtColor.SetFromCOLORREF(m_clBkgTextColor);
			SolidBrush brush(txtColor);  

			CRect rcClient ;  
			GetClientRect(rcClient);  

			//------------------------------------------  
			// SetTextRenderingHint控制文本输出的质量  
			//------------------------------------------  
			graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);  

			//------------------------------------------  
			// 一种是point ,一种是rect控制输出  
			//------------------------------------------  
			RectF destRectf((REAL)rc.left, (REAL)rc.top, (REAL)w, (REAL)h);
			graphics.DrawString(CT2W(m_szBkgText), -1, &font, destRectf, &stringformat, &brush); 
		}
	}
}

BOOL CPictureCtrl::OnEraseBkgnd(CDC *pDC)
{
	if(m_bIsPicLoaded)
	{
		//Get control measures
		RECT rc;
		this->GetClientRect(&rc);

		Graphics graphics(pDC->GetSafeHdc());
		LARGE_INTEGER liSeekPos;
		liSeekPos.QuadPart = 0;
		m_pStream->Seek(liSeekPos, STREAM_SEEK_SET, NULL);
		Image image(m_pStream);

		int w = rc.right-rc.left;
		int h = rc.bottom - rc.top;
		Rect destRect(rc.left, rc.top, w, h);
		if (m_rcShow.IsRectEmpty())
		{
			graphics.DrawImage(&image, destRect);
		}
		else
		{
			graphics.DrawImage(&image, destRect, m_rcShow.left, m_rcShow.top, m_rcShow.Width(), m_rcShow.Height(), Gdiplus::UnitPixel);
		}

		// 画坐标框
		if (m_bShowPos && !m_rcPosition.IsRectEmpty())
		{
			int x1 = (int)(m_rcPosition.left * 1.0 * w / image.GetWidth());
			int y1 = (int)(m_rcPosition.top * 1.0 * h / image.GetHeight());
			int x2 = (int)(m_rcPosition.right * 1.0 * w / image.GetWidth());
			int y2 = (int)(m_rcPosition.bottom * 1.0 * h / image.GetHeight());

			Pen redpen(Color::Red, 2.0f);
			Point pts[5] = {Point(x1, y1),Point(x1, y2),Point(x2, y2),Point(x2, y1),Point(x1, y1)};
			graphics.DrawLines(&redpen,pts,5);
		}
		return TRUE;
	}
	else
	{
		return CStatic::OnEraseBkgnd(pDC);
	}
}

void CPictureCtrl::SetBkgText(LPCTSTR pszText, BOOL bErase)
{
	m_szBkgText = pszText ? pszText : _T("");
	if (bErase)
	{
		Invalidate();
		RedrawWindow();
	}
}

void CPictureCtrl::SetBkgText(LPCTSTR pszText, COLORREF bkColor, COLORREF txtColor, BOOL bErase)
{
	m_clBkgColor     = bkColor;
	m_clBkgTextColor = txtColor;

	SetBkgText(pszText, bErase);
}
