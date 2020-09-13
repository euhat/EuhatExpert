#include "stdafx.h"
#include "CtrlExCommon.h"

HBITMAP ctrlExLoadPictureFromFile(LPCTSTR szFile)
{
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return NULL;

	DWORD dwFileSize = GetFileSize(hFile, NULL);

	LPVOID pvData = NULL;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);

	pvData = GlobalLock(hGlobal);

	DWORD dwBytesRead = 0;
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pStm = NULL;
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStm);

	LPPICTURE gpPicture;

	hr = ::OleLoadPicture(pStm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&gpPicture);

	pStm->Release();

	OLE_HANDLE picHandle;
	gpPicture->get_Handle(&picHandle);
/*	HDC hdc = ::CreateCompatibleDC(NULL);
	::SelectObject(hdc, (HGDIOBJ)picHandle);

	HBITMAP hRet = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
	DeleteDC(hdc);
*/
	HBITMAP hRet = (HBITMAP)picHandle;
//	gpPicture->Release();

	return hRet;
}

HBITMAP ctrlExGetConfigBitmap(LPCTSTR folderName, LPCTSTR fileName)
{
	TCHAR filePath[_MAX_PATH] = { 0 };
	_stprintf_s(filePath, _T("%s\\%s\\%s.jpg"), CTRL_EX_SKIN_PATH, folderName, fileName);

/*	HBITMAP hbmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), filepath, IMAGE_BITMAP, 0, 0,
		LR_DEFAULTCOLOR | LR_LOADFROMFILE | LR_CREATEDIBSECTION);
*/
	HBITMAP hBmp = ctrlExLoadPictureFromFile(filePath);
	return hBmp;
}

int ctrlExGetBitmapWidth(HBITMAP hBitmap)
{
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm);
	return bm.bmWidth;
}

int ctrlExGetBitmapHeight(HBITMAP hBitmap)
{
	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm);
	return bm.bmHeight;
}

COLORREF ctrlExGetConfigColor(LPCTSTR sectName, LPCTSTR keyName)
{
	TCHAR curpath[_MAX_PATH] = { 0 };
	GetCurrentDirectory(_MAX_PATH - 1, curpath);

	TCHAR filepath[_MAX_PATH] = { 0 };
	_stprintf_s(filepath, _T("%s\\%s"), curpath, _T("skin\\colors.ini"));

	TCHAR tempbuf[128] = { 0 };
	GetPrivateProfileString(sectName, keyName, _T("128,128,128"), tempbuf, 128 - 1, filepath);

	int r = 0, g = 0, b = 0;
	_stscanf_s(tempbuf, _T("%d,%d,%d"), &r, &g, &b);

	return RGB(r, g, b);
}

COLORREF ctrlExGetListHdrBkgColor()
{
	return ctrlExGetConfigColor(_T("common"), _T("listHdrBkColor"));
}