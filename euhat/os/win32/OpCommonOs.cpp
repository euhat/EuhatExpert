#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <os/EuhatPath.h>
#include <windows.h>
#include <time.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <common/JyDataStream.h>
#include <os/EuhatFileHandles.h>
#include <EuhatPostDef.h>

#ifdef EUHAT_DEBUG_ENABLE
void ivrLog(const char *format, ...)
{
	va_list argptr;
	char buf[1024 * 2];

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

	time_t t = time(NULL);
	tm *local = localtime(&t);
	char timeBuf[256];
	strftime(timeBuf, 254, "[%Y-%m-%d %H:%M:%S] ", local);
	string dispStr = timeBuf;
	dispStr += buf;

	printf("%s", dispStr.c_str());

	const char *logFileName = "d:\\log.txt";
	const char *logBackupFileName = "d:\\log_bak.txt";

	struct _stat info;
	_stat(logFileName, &info);
	long fileSize = info.st_size;

	if (fileSize > 1024 * 1024 * 100)
	{
		wchar_t wstrBackup[512];
		MultiByteToWideChar(CP_ACP, 0, logBackupFileName, -1, wstrBackup, 511);
		DeleteFileW(wstrBackup);
		wchar_t wstr[512];
		MultiByteToWideChar(CP_ACP, 0, logFileName, -1, wstr, 511);
		MoveFileW(wstr, wstrBackup);
	}

	FILE *fp = fopen(logFileName, "a+");
	if (NULL != fp)
	{
		fprintf(fp, "%s", dispStr.c_str());
		fclose(fp);
	}
	return;
}
#endif

string gbToUtf8(const char *str)
{
	string result;

	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	unique_ptr<WCHAR []> strSrc(new WCHAR[i + 1]);
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc.get(), i);

	i = WideCharToMultiByte(CP_UTF8, 0, strSrc.get(), -1, NULL, 0, NULL, NULL);
	unique_ptr<char []> szRes(new char[i + 1]);
	int j = WideCharToMultiByte(CP_UTF8, 0, strSrc.get(), -1, szRes.get(), i, NULL, NULL);

	result = szRes.get();

	return result;
}

string utf8ToGb(const char *str)
{
	string result;

	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	unique_ptr<WCHAR []> strSrc(new WCHAR[i + 1]);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc.get(), i);

	i = WideCharToMultiByte(CP_ACP, 0, strSrc.get(), -1, NULL, 0, NULL, NULL);
	unique_ptr<char []> szRes(new char[i + 1]);
	WideCharToMultiByte(CP_ACP, 0, strSrc.get(), -1, szRes.get(), i, NULL, NULL);

	result = szRes.get();

	return result;
}

wstring utf8ToWstr(const char *str)
{
	wstring result;

	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	unique_ptr<WCHAR []> strSrc(new WCHAR[i + 1]);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc.get(), i);

	result = strSrc.get();

	return result;
}

string wstrToUtf8(const wchar_t *str)
{
	string result;

	int i = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	unique_ptr<char []> strSrc(new char[i + 1]);
	WideCharToMultiByte(CP_UTF8, 0, str, -1, strSrc.get(), i, NULL, NULL);

	result = strSrc.get();

	return result;
}

wstring gbToWstr(const char *str)
{
	wstring result;

	int i = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	unique_ptr<WCHAR[]> strSrc(new WCHAR[i + 1]);
	MultiByteToWideChar(CP_ACP, 0, str, -1, strSrc.get(), i);

	result = strSrc.get();

	return result;
}

string wstrToGb(const wchar_t *str)
{
	string result;

	int i = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	unique_ptr<char []> strSrc(new char[i + 1]);
	WideCharToMultiByte(CP_ACP, 0, str, -1, strSrc.get(), i, NULL, NULL);

	result = strSrc.get();

	return result;
}

string readIniString(const char *iniFileName, const char *sectionName, const char *name, const char *defaultVal)
{
	char curPath[_MAX_PATH];
	GetCurrentDirectoryA(_MAX_PATH - 1, curPath);

	char filepath[_MAX_PATH];
	sprintf(filepath, "%s\\%s", curPath, iniFileName);

	char keyValue[1024];
	GetPrivateProfileStringA(sectionName, name, defaultVal, keyValue, 1024 - 1, filepath);

	string ret = keyValue;
	return ret;
}

int writeIniString(const char *iniFileName, const char *sectionName, const char *name, const char *value)
{
	char curPath[_MAX_PATH];
	GetCurrentDirectoryA(_MAX_PATH - 1, curPath);

	char filepath[_MAX_PATH];
	sprintf(filepath, "%s\\%s", curPath, iniFileName);

	WritePrivateProfileStringA(sectionName, name, value, filepath);
	return 1;
}

int setRegStr(TCHAR *folder, TCHAR *name, const char *valStr)
{
	int result = 0;
	HKEY hKey;
	if (ERROR_SUCCESS == ::RegCreateKey(HKEY_CURRENT_USER, folder, &hKey))
	{
		if (ERROR_SUCCESS != ::RegSetValueEx(hKey, name, 0, REG_SZ, (const BYTE *)valStr, (int)strlen(valStr) + 1))
		{
			result = -1;
		}
	}

	::RegCloseKey(hKey);
	return result;
}

int getRegStr(TCHAR *folder, TCHAR *name, string &outValStr)
{
	int result = 0;
	HKEY hKey;
	char strValue[1024];
	DWORD dwSzType = REG_SZ;
	DWORD dwSize = sizeof(strValue) - 1;
	strValue[sizeof(strValue) - 1] = 0;
	if (ERROR_SUCCESS == ::RegOpenKey(HKEY_CURRENT_USER, folder, &hKey))
	{
		if (ERROR_SUCCESS != ::RegQueryValueEx(hKey, name, 0, &dwSzType, (LPBYTE)&strValue, &dwSize))
		{
			result = -1;
		}
		else
			outValStr = strValue;
	}

	::RegCloseKey(hKey);
	return result;
}

string getCurrentDir()
{
	char curPath[_MAX_PATH];
	GetCurrentDirectoryA(_MAX_PATH - 1, curPath);

	return curPath;
}

string whCorrectFilePath(const char* path)
{
	EuhatPath pa;
	pa.inStr(path);
	pa.isUnix_ = 0;
	return pa.toStr();
}

int opMkDir(const char *path)
{
	int result = (_wmkdir(utf8ToWstr(path).c_str()) == 0);
	if (!result)
	{
		if (EEXIST == errno)
			return 1;
	}
	return result;
}

int64_t whGetFileSize(const char *path)
{
	WhFileGuard g(EuhatFileHandles::getInstance()->fopen(path, "rb"));
	if (NULL == g.fp_)
	{
		return 0;
	}

	whFseek64(g.fp_, 0, SEEK_END);
	int64_t truePos = whFtell64(g.fp_);
	return truePos;
}

FILE *whFopen(const char *path, const char *mode)
{
	return _wfopen(utf8ToWstr(path).c_str(), utf8ToWstr(mode).c_str());
}

void whFClose(FILE* fp)
{
	fclose(fp);
}

int opUnlink(const char *path)
{
	return _wunlink(utf8ToWstr(path).c_str()) == 0;
}

int opRmDir(const char *path)
{
	return _wrmdir(utf8ToWstr(path).c_str()) == 0;
}

string whPathGetContainer(const char *path)
{
	const char *p;
	for (p = path + strlen(path); p >= path; p--)
	{
		if (*p == '/' || *p == '\\')
			break;
	}
	return string(path).substr(0, p - path + 1);
}

string whPathGetFileName(const char *path)
{
	const char *p;
	for (p = path + strlen(path); p >= path; p--)
	{
		if (*p == '/' || *p == '\\')
			break;
	}
	if (*p == '/' || *p == '\\')
		p++;
	return p;
}

void whDirMakeRecursively(const char *path)
{
	size_t i, len;

	if (NULL == path)
		return;

	char *p = opStrDup(path);

	len = strlen(p);
	for (i = 0; i < len; i++)
	{
		if (p[i] == '\\' || p[i] == '/')
		{
			p[i] = '\0';
			opMkDir(p);
			p[i] = '\\';
		}
	}
	if (len > 0)
		opMkDir(p);

	free(p);
}

int whPrepareFolder(const char *targetPath)
{
	string targetContainerDir = whPathGetContainer(targetPath);
	whDirMakeRecursively(targetContainerDir.c_str());

	return 1;
}

char *opStrDup(const char *str)
{
	return _strdup(str);
}

int doesFileExist(const char *filePath)
{
	int result = _access(filePath, 0);
	if (result < 0)
		return 0;
	return 1;
}

int gettimeofday(struct timeval *tp, void *tzp)
{
	uint64_t  intervals;
	FILETIME  ft;

	GetSystemTimeAsFileTime(&ft);

	/*
	 * A file time is a 64-bit value that represents the number
	 * of 100-nanosecond intervals that have elapsed since
	 * January 1, 1601 12:00 A.M. UTC.
	 *
	 * Between January 1, 1970 (Epoch) and January 1, 1601 there were
	 * 134744 days,
	 * 11644473600 seconds or
	 * 11644473600,000,000,0 100-nanosecond intervals.
	 *
	 * See also MSKB Q167296.
	 */

	intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
	intervals -= 116444736000000000;

	tp->tv_sec = (long)(intervals / 10000000);
	tp->tv_usec = (long)((intervals % 10000000) / 10);

	return 1;
}

string whGetAbsolutePath(const char *path)
{
	if (path == NULL || path[0] == 0)
		return "";
	if (strlen(path) > 1 && path[1] == ':')
		return path;
	string curDir = getCurrentDir();
	return curDir + "\\" + path;
}

void whGetDiskUsage(const char *path, int64_t &totalSpaceInMB, int64_t &freeSpaceInMB)
{
	string absPath = whGetAbsolutePath(path);

	BOOL result;
	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	char dir[4] = { absPath[0], ':', '\\', '\0' };

	result = GetDiskFreeSpaceExA(dir,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);

	if (result)
	{
		totalSpaceInMB = (i64TotalBytes.u.HighPart << 12) + (i64TotalBytes.u.LowPart >> 20);
		freeSpaceInMB = (i64FreeBytes.u.HighPart << 12) + (i64FreeBytes.u.LowPart >> 20);
	}
}

static time_t fileTime2TimeT(FILETIME ft)
{
	//	LONGLONG nLL;
	ULARGE_INTEGER ui;

	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	//	nLL = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;
	time_t t = (long)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);

	return t;
}

time_t whGetFileCreateTime(const char *filePath)
{
	time_t t;
	WIN32_FIND_DATAA findFileData = { 0 };
	HANDLE hFile = FindFirstFileA(filePath, &findFileData);
	if (INVALID_HANDLE_VALUE == hFile)
		return 0;

	t = fileTime2TimeT(findFileData.ftCreationTime);

	FindClose(hFile);

	return t;
}

void whSetFileLastWriteTime(const char *path, time_t t)
{
	SYSTEMTIME systime;
	FILETIME ft, ftUTC;
	HANDLE hFile;

	struct tm *localTime = localtime(&t);
	systime.wYear = localTime->tm_year + 1900;
	systime.wMonth = localTime->tm_mon + 1;
	systime.wDay = localTime->tm_mday;
	systime.wHour = localTime->tm_hour;
	systime.wMinute = localTime->tm_min;
	systime.wSecond = localTime->tm_sec;
	systime.wMilliseconds = 0;

	SystemTimeToFileTime(&systime, &ft);
	LocalFileTimeToFileTime(&ft, &ftUTC);

	hFile = CreateFile(utf8ToWstr(path).c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	SetFileTime(hFile, (LPFILETIME)NULL, (LPFILETIME)NULL, &ftUTC);
	CloseHandle(hFile);
}

int whGetDiskDrivers(JyDataWriteStream &ds)
{
	DWORD dwDisk = GetLogicalDrives();
	char driver[] = "A:";
	for (int dwMask = 1; driver[0] != '['; dwMask *= 2, driver[0]++)
	{
		if (dwDisk & dwMask)
		{
			ds.put<char>(0);
			ds.putStr(driver);
		}
	}
	ds.put<char>(2);
	return 1;
}

int whGetSubFiles(const char *pathUtf8, JyDataWriteStream &ds)
{
	ds.putStr(pathUtf8);

	if (pathUtf8[0] == 0)
		return whGetDiskDrivers(ds);

	wstring strFind;
	WIN32_FIND_DATA findFileData;

	strFind = utf8ToWstr(pathUtf8) + L"\\*.*";

	HANDLE hFind = FindFirstFile(strFind.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return 0;

	while (TRUE)
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (opStrCmp(findFileData.cFileName, L".") != 0 && opStrCmp(findFileData.cFileName, L"..") != 0)
			{
				ds.put<char>(0);
				ds.putStr(wstrToUtf8(findFileData.cFileName).c_str());
			}
		}
		else
		{
			ds.put<char>(1);
			ds.putStr(wstrToUtf8(findFileData.cFileName).c_str());
			ds.put<int64_t>(WH_PACK_INT64(findFileData.nFileSizeHigh, findFileData.nFileSizeLow));
			ds.put<int64_t>(fileTime2TimeT(findFileData.ftLastWriteTime));
		}
		if (!FindNextFile(hFind, &findFileData))
			break;
	}
	FindClose(hFind);

	ds.put<char>(2);

	return 1;
}

HICON whFileIcon(const char *extention, int exFlags)
{
	HICON icon = NULL;

	SHFILEINFO info;
	if (SHGetFileInfo(utf8ToWstr(extention).c_str(),
		FILE_ATTRIBUTE_NORMAL,
		&info,
		sizeof(info),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | exFlags))
	{
		icon = info.hIcon;
	}

	return icon;
}

std::string whFileType(const char *extention)
{
	string type;

	SHFILEINFO info;
	if (SHGetFileInfo(utf8ToWstr(extention).c_str(),
		FILE_ATTRIBUTE_NORMAL,
		&info,
		sizeof(info),
		SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
	{
		type = wstrToUtf8(info.szTypeName);
	}

	return type;
}

HICON whFolderIcon(int exFlags)
{
	HICON icon = NULL;

	SHFILEINFO info;
	if (SHGetFileInfo(L"folder",
		FILE_ATTRIBUTE_DIRECTORY,
		&info,
		sizeof(info),
		SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | exFlags))
	{
		icon = info.hIcon;
	}

	return icon;
}

std::string whFolderType()
{
	std::string type;

	SHFILEINFO info;
	if (SHGetFileInfo(L"folder",
		FILE_ATTRIBUTE_DIRECTORY,
		&info,
		sizeof(info),
		SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
	{
		type = wstrToUtf8(info.szTypeName);
	}

	return type;
}

string whGetHostName()
{
	TCHAR name[MAX_PATH];
	DWORD len = MAX_PATH - 1;
	GetComputerName(name, &len);
	name[len] = 0;
	string str = wstrToUtf8(name);
	return str.c_str();
}

void whGetSysInfo(JyDataWriteStream &ds)
{
	ds.putStr(whGetHostName().c_str());
	ds.put<short>(0); // system type.
//	ds.putStr("");
}

void whUpdateClipboard(const char *msg)
{

}