#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <common/JyDataStream.h>
#include <iconv.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/system_properties.h>
#include <json/json.h>
#include <jni.h>
#include <os/EuhatFileHandles.h>

#ifdef EUHAT_DEBUG_ENABLE
#include <android/log.h>

const char *LOG_TAG = "EuhatLog";

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGT(...) __android_log_print(ANDROID_LOG_INFO, "alert", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

void ivrLog(const char *format, ...)
{
	va_list argptr;
	char buf[1024 * 2];

	va_start(argptr, format);
	vsprintf(buf, format, argptr);
	va_end(argptr);

    struct timeval tv;
    int iRet = gettimeofday(&tv, NULL);
	time_t t = tv.tv_sec;
	tm* local = localtime(&t);
	char timeBuf[256];
	strftime(timeBuf, 254, "[%Y-%m-%d %H:%M:%S", local);
	sprintf(timeBuf + strlen(timeBuf), ":%d] ", (int)(tv.tv_usec / 1000));
	string dispStr = timeBuf;
	dispStr += buf;

	LOGE(dispStr.c_str());

	return;

	printf("%s", dispStr.c_str());

    char *logFileName = (char *)"/sdcard/0test/log.txt";

    int fd = open(logFileName, O_CREAT|O_WRONLY|O_APPEND, 0666);
    int err = errno;
    if (fd != -1)
    {
        write(fd, dispStr.c_str(), dispStr.length());
        close(fd);
    }

}
#endif

static int codeConvert(const char *fromCharSet, const char *toCharSet, const char *inBuf, size_t inlen,
					   char *outBuf, size_t outLen)
{
#if 0
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
#endif
	return 0;
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
#if 0
	char curPath[1024];
	getcwd(curPath, sizeof(curPath) - 1);

	return curPath;
#else
	return EUHAT_PATH_BASE;
#endif
}

int opMkDir(const char *path)
{
	int ret = (mkdir(path, 0755) == 0);
	if (!ret)
	{
		int err = errno;
		if (err == EEXIST)
			return 1;
	}
	return ret;
}

int64_t whGetFileSize(const char *path)
{
	struct stat64 st;
	stat64(path, &st);
	return st.st_size;
}

FILE *whFopen(const char *path, const char *mode)
{
	return fopen(path, mode);
}

void whFClose(FILE *fp)
{
    fclose(fp);
}

int whFseek64(FILE *stream, int64_t offset, int origin)
{
	if (feof(stream))
	{
		rewind(stream);
	}
	else
    {
		setbuf(stream, NULL); //清空fread的缓存
	}
	int fd = fileno(stream);
	if (lseek64(fd, offset, origin) == -1)
	{
		return errno;
	}
	return 0;
}

int64_t whFtell64(FILE *stream)
{
	assert("system not implemented.");
	return -1;
}

int opUnlink(const char *filePath)
{
	string path = whGetAbsolutePath(filePath);
	return unlink(path.c_str()) == 0;
}

int opRmDir(const char *filePath)
{
	string path = whGetAbsolutePath(filePath);
	return rmdir(path.c_str()) == 0;
}

string whPathGetContainer(const char *path)
{
	const char *p;
	for (p = path + strlen(path); p >= path; p--)
	{
		if (*p == '/')
			break;
	}
	return string(path).substr(0, p - path + 1);
}

string whPathGetFileName(const char *path)
{
	const char *p;
	for (p = path + strlen(path); p >= path; p--)
	{
		if (*p == '/')
			break;
	}
	if (*p == '/')
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
		if (p[i] == '/')
		{
			p[i] = '\0';
			opMkDir(p);
			p[i] = '/';
		}
	}
	if (len > 0)
		opMkDir(p);

	free(p);
}

int whPrepareFolder(const char *targetPath)
{
	string path = whGetAbsolutePath(targetPath);
	string targetContainerDir = whPathGetContainer(path.c_str());
	whDirMakeRecursively(targetContainerDir.c_str());

	return 1;
}

char *opStrDup(const char *str)
{
	return strdup(str);
}

int doesFileExist(const char *filePath)
{
	string path = whGetAbsolutePath(filePath);
	int result = access(path.c_str(), 0);
	if (result < 0)
		return 0;
	return 1;
}

string whGetAbsolutePath(const char *path)
{
	if (path[0] == '/')
		return path;
	return getCurrentDir() + path;
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

void whSetFileLastWriteTime(const char *path, time_t t)
{

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

string whGetHostName()
{
	char value[93] = "";
	__system_property_get("ro.product.model", value);
	return value;
}

void whGetSysInfo(JyDataWriteStream &ds)
{
	ds.putStr(whGetHostName().c_str());
	ds.put<short>(1); // system type.
//	ds.putStr("/sdcard");
	//android:requestLegacyExternalStorage="true"
    ds.putStr(EUHAT_PATH_FILES);
}
