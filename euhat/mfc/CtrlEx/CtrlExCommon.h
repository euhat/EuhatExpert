#pragma once

#define CTRL_EX_SKIN_PATH _T("skin")

HBITMAP ctrlExGetConfigBitmap(LPCTSTR folderName, LPCTSTR fileName);
int ctrlExGetBitmapWidth(HBITMAP hBitmap);
int ctrlExGetBitmapHeight(HBITMAP hBitmap);
COLORREF ctrlExGetConfigColor(LPCTSTR sectName, LPCTSTR keyName);
COLORREF ctrlExGetListHdrBkgColor();

