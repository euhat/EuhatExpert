#include <EuhatPreDef.h>
#include <windows.h>
#include "OpWin.h"
#include <common/OpCommon.h>
#include <EuhatPostDef.h>

int mySystemNoWait(const char *cmdLine, int showIt)
{
	vector<string> strVec;
	splitTokenString(cmdLine, ' ', strVec);
	const char *exe = strVec[0].c_str();
	wstring cstrExe = gbToWstr(exe);
	const char *param = cmdLine + strlen(exe) + 1;
	wstring cstrParam = gbToWstr(param);

	SHELLEXECUTEINFO shExecInfo = { 0 };
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = NULL;
	shExecInfo.lpFile = cstrExe.c_str();
	shExecInfo.lpParameters = cstrParam.c_str();
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = showIt;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

	CloseHandle(shExecInfo.hProcess);

	return 1;
}

int mySystem(const char *cmdLine, int showIt, DWORD timeOut)
{
	vector<string> strVec;
	splitTokenString(cmdLine, ' ', strVec);
	const char *exe = strVec[0].c_str();
	wstring cstrExe = gbToWstr(exe);
	const char *param = cmdLine + strlen(exe) + 1;
	wstring cstrParam = gbToWstr(param);

	SHELLEXECUTEINFO shExecInfo = { 0 };
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = NULL;
	shExecInfo.lpFile = cstrExe.c_str();
	shExecInfo.lpParameters = cstrParam.c_str();
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = showIt;
	shExecInfo.hInstApp = NULL;
	ShellExecuteEx(&shExecInfo);

	if (WAIT_TIMEOUT == WaitForSingleObject(shExecInfo.hProcess, timeOut))
	{
		TerminateProcess(shExecInfo.hProcess, 0);
		CloseHandle(shExecInfo.hProcess);
		return 0;
	}
	CloseHandle(shExecInfo.hProcess);
	return 1;
}

void fillGradientRect(HDC hdc, RECT *rc, COLORREF fromRGB, COLORREF toRGB, int fillStyle)
{
	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = rc->right - rc->left;
	bmpInfo.bmiHeader.biHeight = rc->bottom - rc->top;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	BYTE *rgbBuffer = (BYTE *)malloc(bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biHeight * 4);

	int width = bmpInfo.bmiHeader.biWidth;
	int height = bmpInfo.bmiHeader.biHeight;

	int rStart = fromRGB & 0xFF;
	int rEnd = toRGB & 0xFF;
	int gStart = (fromRGB & 0xFF00) >> 8;
	int gEnd = (toRGB & 0xFF00) >> 8;
	int bStart = (fromRGB & 0xFF0000) >> 16;
	int bEnd = (toRGB & 0xFF0000) >> 16;
	if (FILL_STYLE_VERTICAL == fillStyle)
	{
		double rStep = (rEnd - rStart) * 1.0 / height;
		double gStep = (gEnd - gStart) * 1.0 / height;
		double bStep = (bEnd - bStart) * 1.0 / height;
		for (int y = 0; y < height; y++)
		{
			BYTE r = (BYTE)(rStart + rStep * y);
			BYTE g = (BYTE)(gStart + gStep * y);
			BYTE b = (BYTE)(bStart + bStep * y);
			for (int x = 0; x < width; x++)
			{
				BYTE *p = rgbBuffer + ((height - y) * width - (width - x)) * 4;
				*p++ = b;
				*p++ = g;
				*p++ = r;
			}
		}
	}
	else
	{
		double rStep = (rEnd - rStart) * 1.0 / width;
		double gStep = (gEnd - gStart) * 1.0 / width;
		double bStep = (bEnd - bStart) * 1.0 / width;
		for (int x = 0; x < width; x++)
		{
			BYTE r = (BYTE)(rStart + rStep * x);
			BYTE g = (BYTE)(gStart + gStep * x);
			BYTE b = (BYTE)(bStart + bStep * x);
			for (int y = 0; y < height; y++)
			{
				BYTE *p = rgbBuffer + ((height - y) * width - (width - x)) * 4;
				*p++ = b;
				*p++ = g;
				*p++ = r;
			}
		}
	}

	StretchDIBits(hdc, rc->left, rc->top, width, height, 0, 0, width, height, rgbBuffer, &bmpInfo, DIB_RGB_COLORS, SRCCOPY);

	free(rgbBuffer);
}