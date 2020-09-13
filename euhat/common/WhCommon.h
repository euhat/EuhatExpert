#pragma once

#include <WhCommonOs.h>

#define WhSockAddr sockaddr
#define WhSockAddrIn sockaddr_in
#define WhSockLenT int
#define whCondWaitTimed(_handle, _mutex, _secs) whCondWaitTimedMsec(_handle, _mutex, _secs * 1000)

#define WH_PACK_INT64(_dwHigh,_dwLow) (((int64_t)(_dwHigh)) << 32 | ((int64_t)(_dwLow)))

FILE *whFopen(const char *path, const char *mode);
int64_t whGetFileSize(const char *path);
string whGetAbsolutePath(const char *path);
void whGetDiskUsage(const char *path, int64_t &totalSpaceInMB, int64_t &freeSpaceInMB);
time_t whGetFileCreateTime(const char *filePath);
void whSetFileLastWriteTime(const char *path, time_t t);

string whPathGetContainer(const char *path);
string whPathGetFileName(const char *path);
void whDirMakeRecursively(const char *path);
int whPrepareFolder(const char *targetPath);

class JyDataWriteStream;

int whGetSubFiles(const char *path, JyDataWriteStream &ds);
string whGetHostName();
void whGetSysInfo(JyDataWriteStream &ds);

struct WhTimeVal
{
	timeval t_;
	WhTimeVal &now()
	{
		gettimeofday(&t_, NULL);
		return *this;
	}
	WhTimeVal &add(WhTimeVal &r)
	{
		t_.tv_sec += r.t_.tv_sec;
		t_.tv_usec += r.t_.tv_usec;
		return *this;
	}
	WhTimeVal &sub(WhTimeVal &r)
	{
		t_.tv_sec -= r.t_.tv_sec;
		t_.tv_usec -= r.t_.tv_usec;
		return *this;
	}
	int biggerThan(WhTimeVal &r)
	{
		return t_.tv_sec > r.t_.tv_sec || (t_.tv_sec == r.t_.tv_sec && t_.tv_usec > r.t_.tv_usec);
	}
	int64_t calcInt64()
	{
		int64_t t = t_.tv_sec;
		t *= 1000;
		t += t_.tv_usec % 1000;
		return t;
	}
};

class WhFileGuard
{
public:
	WhFileGuard(FILE *fp)
	{
		fp_ = fp;
	}
	~WhFileGuard()
	{
		if (NULL != fp_)
			fclose(fp_);
	}
	FILE *fp_;
};