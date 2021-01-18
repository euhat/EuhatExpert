#include <common/OpCommon.h>
#include <common/JyDataStream.h>
#include <iconv.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

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

}
#endif

static int codeConvert(const char *fromCharSet, const char *toCharSet, const char *inBuf, size_t inlen,  
        char *outBuf, size_t outLen)
{  
    iconv_t cd;
    char **pIn = (char **)&inBuf;
    char **pOut = &outBuf;
  
    cd = iconv_open(toCharSet, fromCharSet);
    if (cd == NULL)
        return 0;
	
    if (iconv(cd, pIn, &inlen, pOut, &outLen) == -1)
	{
		outLen = 0;
        goto out;
	}
	
out:
    iconv_close(cd);
//	*pOut = '\0';
  
    return outLen;
}  

string gbToUtf8(const char *str)
{
	char buf[1024];
	
	codeConvert("gb2312", "utf-8", str, strlen(str) + 1, buf, sizeof(buf) - 1);  

	return buf;
}

string utf8ToGb(const char *str)
{
	char buf[1024];
	
	codeConvert("utf-8", "gb2312", str, strlen(str) + 1, buf, sizeof(buf) - 1);  

	return buf;
}

wstring utf8ToWstr(const char *str)
{
	return wstring();
}

string wstrToUtf8(const wchar_t *str)
{
	return "";
}

string readIniString(const char *iniFileName, const char *sectionName, const char *name, const char *defaultVal)
{
	return "";
}

int writeIniString(const char *iniFileName, const char *sectionName, const char *name, const char *value)
{
	return 0;
}

string getCurrentDir()
{
	char curPath[1024];
	getcwd(curPath, sizeof(curPath) - 1);
	
	return curPath;
}

int opMkDir(const char *path)
{
	return mkdir(path, 0755);
}

int64_t whGetFileSize(const char *path)
{
	struct stat64 st;
	stat64(path, &st);
	return st.st_size;
}

int opUnlink(const char *path)
{
	return unlink(path);
}

char *opStrDup(const char *str)
{
	return strdup(str);
}

int doesFileExist(const char *filePath)
{
	int result = access(filePath, 0);
	if (result < 0)
		return 0;
	return 1;
}

string whGetAbsolutePath(const char *path)
{
	if (path[0] == '/')
		return path;
	return getCurrentDir() + '/' + path;
}

void whGetDiskUsage(const char *path, int64_t &totalSpaceInMB, int64_t &freeSpaceInMB)
{
    struct statfs diskInfo;
    statfs(path, &diskInfo);

    unsigned long long blocksize = diskInfo.f_bsize; //每个block里包含的字节数
    unsigned long long totalsize = blocksize * diskInfo.f_blocks; //总的字节数，f_blocks为block的数目
/*  printf("Total_size = %llu B = %llu KB = %llu MB = %llu GB\n",
           totalsize, totalsize>>10, totalsize>>20, totalsize>>30);
*/
    totalSpaceInMB = totalsize >> 20;

    unsigned long long freeDisk = diskInfo.f_bfree * blocksize; //剩余空间的大小
    unsigned long long availableDisk = diskInfo.f_bavail * blocksize; //可用空间大小
/*  printf("Disk_free = %llu MB = %llu GB\nDisk_available = %llu MB = %llu GB\n",
           freeDisk>>20, freeDisk>>30, availableDisk>>20, availableDisk>>30);
*/
    //freeSpaceInMB = freeDisk >> 20;
    freeSpaceInMB = availableDisk >> 20;
}

time_t whGetFileCreateTime(const char *filePath)
{
    struct stat s;
    int result;

    result = lstat(filePath, &s);

    if (result == -1)
        return 0;

    return s.st_ctime;
//    return s.st_atime;
//    return s.st_mtime;
}

int whGetSubFiles(const char *path, JyDataWriteStream &ds)
{
	DIR *dir;
	dir = opendir(path);
	if (dir == NULL)
		return 0;

	ds.putStr(path);

	struct dirent *dirInfo;
	while ((dirInfo = readdir(dir)) != NULL)
	{
		if (strcmp(dirInfo->d_name, "..") == 0)
			continue;
		if (strcmp(dirInfo->d_name, ".") == 0)
			continue;

		string tmpPath = string(path) + "/" + dirInfo->d_name;

		struct stat st;
		stat(tmpPath.c_str(), &st);
		if (S_ISDIR(st.st_mode))
		{
			ds.put<char>(0);
			ds.putStr(dirInfo->d_name);
		}
		else
		{
			ds.put<char>(1);
			ds.putStr(dirInfo->d_name);
			ds.put<int64_t>(whGetFileSize(tmpPath.c_str()));
			ds.put<int64_t>((int64_t)whGetFileCreateTime(tmpPath.c_str()));
		}
	}
	closedir(dir);

	ds.put<char>(2);
	return 1;
}