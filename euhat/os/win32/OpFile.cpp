#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <Windows.h>
#include <direct.h>
#include <time.h>
#include <io.h>
#include "OpFile.h"
#include <EuhatPostDef.h>

void runThroughtFiles(const char *lpPath, list<RunThroughtFilesResult> &files)
{
	string strFind;

	WIN32_FIND_DATAA findFileData;

	strFind = string(lpPath) + "\\*.*";

	HANDLE hFind = FindFirstFileA(strFind.c_str(), &findFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	while (TRUE)
	{
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0)
			{
				string strSubPath;

				strSubPath = string(lpPath) + "\\" + findFileData.cFileName;

				runThroughtFiles(strSubPath.c_str(), files);
			}
		}
		else
		{
			RunThroughtFilesResult result;
			result.filePath = string(lpPath) + "\\" + findFileData.cFileName;
			result.createTime_ = whGetFileCreateTime(result.filePath.c_str());
			result.filePath = strToLower(result.filePath.c_str());
			files.push_back(result);
		}
		if (!FindNextFileA(hFind, &findFileData))
			break;
	}
	FindClose(hFind);
}

void removeEmptyFolderRecursive(const char *dir)
{
	string strFind;
	int subDirCount = 0;
	int subFileCount = 0;
	WIN32_FIND_DATAA FindFileData;

	strFind = string(dir) + "\\*.*";

	HANDLE hFind = FindFirstFileA(strFind.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	while (TRUE)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				string strSubPath;

				strSubPath = string(dir) + "\\" + FindFileData.cFileName;

				removeEmptyFolderRecursive(strSubPath.c_str());

				if (ENOENT != _access(strSubPath.c_str(), 00))
					subDirCount++;
			}
		}
		else
		{
			subFileCount++;
		}
		if (!FindNextFileA(hFind, &FindFileData))
			break;
	}
	if (subFileCount == 0 && subDirCount == 0)
	{
		_rmdir(dir);
	}
	FindClose(hFind);
}

void removeEmptyFolder(const char *dir)
{
	removeEmptyFolderRecursive(dir);
	_mkdir(dir);
}

int RemoveOldFilesParam::init(const char *param)
{
	if (strIsEndWith(param, "d"))
	{
		style = 0;
		days = atoi(param);
	}
	else
	{
		style = 1;
		gigas = atoi(param);
	}
	return 1;
}

string RemoveOldFilesParam::toString()
{
	char buf[1024];

	if (style == 0)
	{
		sprintf(buf, "删除%d天以前文件", days);
	}
	else
	{
		sprintf(buf, "删除旧文件使磁盘剩余空间大于%dg", gigas);
	}

	return buf;
}

int getDiskUsage(const char driveLetter, int &totalSpace, int &freeSpace)
{
	BOOL result;
	ULARGE_INTEGER i64FreeBytesToCaller;
	ULARGE_INTEGER i64TotalBytes;
	ULARGE_INTEGER i64FreeBytes;
	char dir[4] = { driveLetter, ':', '\\', '\0' };

	result = GetDiskFreeSpaceExA(dir,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);

	if (result)
	{
		totalSpace = (i64TotalBytes.u.HighPart << 12) + (i64TotalBytes.u.LowPart >> 20);
		totalSpace >>= 10;
		freeSpace = (i64FreeBytes.u.HighPart << 12) + (i64FreeBytes.u.LowPart >> 20);
		freeSpace >>= 10;
		return 1;
	}

	return 0;
}

void cleanOldFiles(const char *dir, RemoveOldFilesParam *param)
{
	DBG(("开始删除[%s]中的旧文件。\n", dir));

	time_t tn = time(NULL);

	list<RunThroughtFilesResult> sourceFiles;

	runThroughtFiles(dir, sourceFiles);

	if (param->style == 0)
	{
		int totalRemoved = 0;

		unsigned int secInterval = param->days * 24 * 60 * 60;
		for (list<RunThroughtFilesResult>::iterator it = sourceFiles.begin(); it != sourceFiles.end(); it++)
		{
			if (tn - it->createTime_ > secInterval)
			{
				DeleteFileA(it->filePath.c_str());
				totalRemoved++;
			}
		}
		DBG(("按删除%d天以前的文件的规则，删除了%d个文件。\n", param->days, totalRemoved));
	}
	else
	{
		int totalRemoved = 0;

		char driverLetter = 'c';
		if (strlen(dir) > 1 && dir[1] == ':')
			driverLetter = dir[0];
		else
		{
			char curPath[_MAX_PATH];
			GetCurrentDirectoryA(_MAX_PATH - 1, curPath);
			driverLetter = curPath[0];
		}

		sourceFiles.sort();
		
		DBG(("完成%d个文件的排序。\n", (int)sourceFiles.size()));

		while (1)
		{
			int totalSpace;
			int freeSpace;
			if (!getDiskUsage(driverLetter, totalSpace, freeSpace))
				break;
			if (freeSpace > param->gigas)
				break;
			int freedCount = 0;
			while (freedCount < 100)
			{
				if (sourceFiles.size() == 0)
					break;
				RunThroughtFilesResult fileResult = sourceFiles.front();
				sourceFiles.pop_front();
				DeleteFileA(fileResult.filePath.c_str());
				freedCount++;
				totalRemoved++;
			}
			if (sourceFiles.size() == 0)
				break;
		}

		DBG(("按以保持最小%dGB的规则，删除了%d文件。\n", param->gigas, totalRemoved));
	}

	removeEmptyFolder(dir);
}

DWORD getFileSize(const char *filePath)
{
	WIN32_FIND_DATAA findFileData = { 0 };
	HANDLE hFile = FindFirstFileA(filePath, &findFileData);
	if (INVALID_HANDLE_VALUE == hFile)
		return 0;

	DWORD fileSize = findFileData.nFileSizeLow;

	FindClose(hFile);

	return fileSize;
}

