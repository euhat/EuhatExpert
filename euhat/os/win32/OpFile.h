#pragma once

#include <common/OpCommon.h>
#include <list>

struct RunThroughtFilesResult
{
	string filePath;
	time_t createTime_;

	bool operator < (RunThroughtFilesResult &r)
	{
		return createTime_ < r.createTime_;
	}
};

void runThroughtFiles(const char *lpPath, list<RunThroughtFilesResult> &files);

struct RemoveOldFilesParam
{
	int style;
	int days;
	int gigas;

	int init(const char *param);
};

int getDiskUsage(const char driveLetter, int &totalSpace, int &freeSpace);
void cleanOldFiles(const char *dir, RemoveOldFilesParam *param);

DWORD getFileSize(const char *filePath);
