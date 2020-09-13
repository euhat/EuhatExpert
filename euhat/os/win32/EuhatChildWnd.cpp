#include <EuhatPreDef.h>
#include <windows.h>
#include <Windowsx.h>
#include <tchar.h>
#include "EuhatChildWnd.h"
#include <EuhatPostDef.h>

namespace euhatwin
{
	void drawLine(HDC hdc, int startX, int startY, int endX, int endY)
	{
		::MoveToEx(hdc, startX, startY, NULL);
		::LineTo(hdc, endX, endY);
	}

	void drawRect(HDC hdc, RECT &rc)
	{
		::MoveToEx(hdc, rc.left, rc.top, NULL);
		::LineTo(hdc, rc.right - 1, rc.top);
		::LineTo(hdc, rc.right - 1, rc.bottom - 1);
		::LineTo(hdc, rc.left, rc.bottom - 1);
		::LineTo(hdc, rc.left, rc.top);
	}
}

EuhatRect::EuhatRect()
{
}

EuhatRect::EuhatRect(RECT &rc)
{
	rc_ = rc;
}

EuhatRect::EuhatRect(int left, int top, int right, int bottom)
{
	rc_.left = left;
	rc_.top = top;
	rc_.right = right;
	rc_.bottom = bottom;
}

int EuhatRect::isIn(int x, int y)
{
	if (rc_.left <= x && x < rc_.right &&
		rc_.top <= y && y < rc_.bottom)
		return 1;
	return 0;
}

void EuhatRect::move(int offsetX, int offsetY)
{
	rc_.left += offsetX;
	rc_.right += offsetX;
	rc_.top += offsetY;
	rc_.bottom += offsetY;
}

void EuhatRect::grow(int widthStep, int heightStep)
{
	rc_.left -= widthStep;
	rc_.right += widthStep;
	rc_.top -= heightStep;
	rc_.bottom += heightStep;
}

void EuhatRect::positive()
{
	if (rc_.left > rc_.right)
	{
		swap(rc_.left, rc_.right);
	}
	if (rc_.top > rc_.bottom)
	{
		swap(rc_.top, rc_.bottom);
	}
}

int EuhatRect::isIntersect(RECT &rc)
{
	int centerDistX = abs(rc_.left + rc_.right - rc.left - rc.right);
	int centerDistY = abs(rc_.top + rc_.bottom - rc.top - rc.bottom);
	int lenSumX = abs(rc_.right - rc_.left) + abs(rc.right - rc.left);
	int lenSumY = abs(rc_.bottom - rc_.top) + abs(rc.bottom - rc.top);
	if (centerDistX < lenSumX && centerDistY < lenSumY)
		return 1;
	if (centerDistX == lenSumX && centerDistY < lenSumY && rc_.right == rc.left)
		return 1;
	if (centerDistX < lenSumX && centerDistY == lenSumY && rc_.bottom == rc.top)
		return 1;
	return 0;
}

int EuhatRect::width()
{
	return rc_.right - rc_.left;
}

int EuhatRect::height()
{
	return rc_.bottom - rc_.top;
}

void EuhatRect::setWidthHeight(int width, int height)
{
	rc_.right = rc_.left + width;
	rc_.bottom = rc_.top + height;
}

EuhatFont::EuhatFont(const wchar_t *fontName, int fontHeight, int fontWidth)
{
	handle_ = ::CreateFont(
		fontHeight,                // nHeight
		fontWidth,                 // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		fontName);                 // lpszFacename
	height_ = fontHeight;
}

EuhatFont::~EuhatFont()
{
	DeleteObject(handle_);
}


EuhatPen::EuhatPen(COLORREF rgb, int thick, int type)
{
	handle_ = CreatePen(type, thick, rgb);
}

EuhatPen::~EuhatPen()
{
	DeleteObject(handle_);
}

void EuhatPen::drawLine(HDC hdc, int startX, int startY, int endX, int endY)
{
	EuhatGuard<EuhatPen> penGuard(this, hdc);
	euhatwin::drawLine(hdc, startX, startY, endX, endY);
}

void EuhatPen::drawRect(HDC hdc, RECT &rc)
{
	EuhatGuard<EuhatPen> penGuard(this, hdc);
	euhatwin::drawRect(hdc, rc);
}

EuhatBrush::EuhatBrush(COLORREF rgb)
{
	handle_ = CreateSolidBrush(rgb);
}

EuhatBrush::~EuhatBrush()
{
	DeleteObject(handle_);
}

void EuhatBrush::FillRect(HDC hdc, RECT &rc)
{
	EuhatGuard<EuhatBrush> brushGuard(this, hdc);
	::FillRect(hdc, &rc, handle_);
}

LRESULT CALLBACK EuhatChildWnd::euhatChildWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EuhatChildWnd *pThis = NULL;
	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<EuhatChildWnd *>(lpcs->lpCreateParams);
//		pThis->m_hWnd = hWnd;
		pThis->setProp((EuhatChildUserData)pThis);
	} else {
#if EUHAT_CHILD_WND_USE_PROP
		pThis = (EuhatChildWnd *)GetProp(hWnd, EUHAT_CHILD_WND_PROP_NAME);
#else
		pThis = reinterpret_cast<EuhatChildWnd *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
#endif
		if (uMsg == WM_NCDESTROY && pThis != NULL) {
			LRESULT lRes = ::CallWindowProc(pThis->parentWndProc_, hWnd, uMsg, wParam, lParam);
//			SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
//			if (pThis->m_bSubclassed) pThis->Unsubclass();
//			pThis->m_hWnd = NULL;
//			pThis->OnFinalMessage(hWnd);
			return lRes;
		}
	}
	if (pThis != NULL) {
		LRESULT ret;

		DBG(("child wnd msg: %x\n", uMsg));
		if (pThis->hwnd_ == hWnd) {
			return pThis->handleMsg(uMsg, wParam, lParam);
		}

//		pThis->setProp(pThis->parentUserData_);
		ret = CallWindowProc(pThis->parentWndProc_, hWnd, uMsg, wParam, lParam);
//		pThis->setProp((EuhatChildUserData)pThis);

		return ret;
	} else {

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

EuhatChildUserData EuhatChildWnd::getProp()
{
#ifdef EUHAT_CHILD_WND_USE_PROP
	return GetProp(hwnd_, EUHAT_CHILD_WND_PROP_NAME);
#else
	return GetWindowLongPtr(EUHAT_CHILD_WND_HOOK_HWND, GWL_USERDATA);
#endif
}

void EuhatChildWnd::setProp(EuhatChildUserData useData)
{
#ifdef EUHAT_CHILD_WND_USE_PROP
	SetProp(hwnd_, EUHAT_CHILD_WND_PROP_NAME, useData);
#else
	SetWindowLongPtr(EUHAT_CHILD_WND_HOOK_HWND, GWL_USERDATA, useData);
#endif
}

LRESULT CALLBACK EuhatChildWnd::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EuhatChildWnd *pThis = NULL;
	if (uMsg == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<EuhatChildWnd *>(lpcs->lpCreateParams);
		pThis->setProp((EuhatChildUserData)pThis);
	}
	else
	{
#if EUHAT_CHILD_WND_USE_PROP
		pThis = (EuhatChildWnd *)GetProp(hWnd, EUHAT_CHILD_WND_PROP_NAME);
#else
		pThis = reinterpret_cast<EuhatChildWnd *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
#endif
	}
	if (pThis != NULL)
	{
		LRESULT ret = 0;

		if (pThis->hwnd_ == hWnd)
		{
			return pThis->handleMsg(uMsg, wParam, lParam);
		}
		else
		{
			//DBG(("not my msg: %x\n", uMsg));
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void EuhatChildWnd::registerClass(LPCTSTR lpszClass, HINSTANCE hInst)
{
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = EuhatChildWnd::windowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hInstance = hInst;
	wc.lpszClassName = lpszClass;
	wc.style = CS_DBLCLKS;
	RegisterClassEx(&wc);
}

EuhatChildWnd::EuhatChildWnd(HWND parentHwnd)
{
	startPos_.x = startPos_.y = 0;

	parentHwnd_ = parentHwnd;

	HINSTANCE hInst = GetModuleHandle(0);

#if 0
	TCHAR clsName[1024];
	GetClassName(parentHwnd_, clsName, 1023);

	hwnd_ = CreateWindowEx(0, clsName, _T(""), WS_CHILDWINDOW,
		0, 0, 0, 0, parentHwnd_, NULL, hInst, this);

	parentWndProc_ = (WNDPROC)SetWindowLongPtr(hwnd_, GWLP_WNDPROC, (LPARAM)(WNDPROC)(euhatChildWndProc));
#else
	wstring clsName = L"432054343";
	registerClass(clsName.c_str(), hInst);
	hwnd_ = CreateWindowEx(0, clsName.c_str(), _T(""), WS_CHILDWINDOW,
		0, 0, 0, 0, parentHwnd_, NULL, hInst, this);
#endif

	parentUserData_ = getProp();
	setProp((EuhatChildUserData)this);

	ShowWindow(hwnd_, SW_SHOW);
}

EuhatChildWnd::~EuhatChildWnd()
{
	if (NULL == hwnd_)
		return;

	DestroyWindow(hwnd_);
	hwnd_ = NULL;

//	SetWindowLongPtr(parentHwnd_, GWLP_WNDPROC, (LPARAM)(WNDPROC)(parentWndProc_));

	setProp(parentUserData_);
}

LRESULT EuhatChildWnd::handleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
/*	case WM_NCHITTEST:
		return HTCLIENT;
*/	case WM_PAINT: {
		PAINTSTRUCT ps = { 0 };
		BeginPaint(hwnd_, &ps);

		onDraw(ps.hdc);

		EndPaint(hwnd_, &ps);
		return 1;
	}
	case WM_ERASEBKGND: {
		return TRUE;
	}
/*	case WM_WINDOWPOSCHANGED: {
		InvalidateRect(hwnd_, NULL, 1);
		return TRUE;
	}
*/	case WM_LBUTTONDOWN: {
		SetFocus(hwnd_);
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		onLBtnDown(wParam, xPos, yPos);
		break;
	}
	case WM_MOUSEMOVE: {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		onMouseMove(wParam, xPos, yPos);
		break;
	}
	case WM_LBUTTONUP: {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		onLBtnUp(wParam, xPos, yPos);
		break;
	}
	case WM_LBUTTONDBLCLK: {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		onDbClick(wParam, xPos, yPos);
		break;
	}
	case WM_MOUSEWHEEL: {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		onMouseWheel(zDelta, fwKeys, xPos, yPos);
		break;
	}
	}
	return DefWindowProc(hwnd_, uMsg, wParam, lParam);;
}

void EuhatChildWnd::onDraw(HDC hdc)
{
	
}

void EuhatChildWnd::onLBtnDown(WPARAM wParam, int x, int y)
{

}

void EuhatChildWnd::onMouseMove(WPARAM wParam, int x, int y)
{

}

void EuhatChildWnd::onLBtnUp(WPARAM wParam, int x, int y)
{

}

void EuhatChildWnd::onDbClick(WPARAM wParam, int x, int y)
{

}

void EuhatChildWnd::onMouseWheel(int zDelta, int fwKeys, int xPos, int yPos)
{
}

void EuhatChildWnd::moveWindow(RECT *rect)
{
	MoveWindow(hwnd_, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, 0);
//	PostMessage(hwnd_, WM_SIZE, 0, 0);
	InvalidateRect(hwnd_, NULL, 1);
}