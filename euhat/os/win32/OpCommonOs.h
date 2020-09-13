#pragma once

int setRegStr(TCHAR *folder, TCHAR *name, const char *valStr);
int getRegStr(TCHAR *folder, TCHAR *name, string &outValStr);

void copyStr2Clipboard(HWND hwndOwner, string src);

HICON whFileIcon(const char *extention, int extFlags = SHGFI_SMALLICON);
std::string whFileType(const char *extention);
HICON whFolderIcon(int extFlags = SHGFI_SMALLICON);
std::string whFolderType();