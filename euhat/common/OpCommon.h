#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#ifdef _DEBUG
#define EUHAT_DEBUG_ENABLE
#endif

#ifdef EUHAT_DEBUG_ENABLE
void ivrLog(const char *format, ...);
#define DBG(_param) ivrLog _param
#else
#define DBG(_param) printf _param
#endif

#ifdef X64
typedef int64_t vint;
#else
typedef int vint;
#endif

#define DEFAULT_INI_FILENAME "config.ini"
#define DEFAULT_SQLITE_DB_FILENAME "euhat.dat"
#define DEFAULT_SQLITE_INI_TABLE_NAME "Config"

#define HAS_ATTR(_flags,_attr) ((_flags) & (_attr))
#define HAS_NO_ATTR(_flags,_attr) ((_flags) | ~(_attr))

string gbToUtf8(const char *str);
string utf8ToGb(const char *str);
wstring utf8ToWstr(const char *str);
string wstrToUtf8(const wchar_t *str);
wstring gbToWstr(const char *str);
string wstrToGb(const wchar_t *str);

int memToFile(const char *path, const char *pData, int iLen);
int memFromFile(const char *path, char *pData, int iLen);
char *memFromWholeFile(const char *path, unsigned int *len);
int loadKVMapFromFile(const char *filePath, map<string, string> &kvMap);
int saveKVMapToFile(const char *filePath, map<string, string> &kvMap);

string getCurrentDir();
string readIniString(const char *iniFileName, const char *sectionName, const char *name, const char *defaultVal = "");
int readIniInt(const char *iniFileName, const char *sectionName, const char *name, int defaultVal = 0);
int writeIniString(const char *iniFileName, const char *sectionName, const char *name, const char *value);
int writeIniInt(const char *iniFileName, const char *sectionName, const char *name, int value);
string intToString(int i);
string int64ToString(int64_t i);

string strToLower(const char *str);
int strIsEndWith(const char *str, const char *endStr);
int strIsStartWith(const char *str, const char *startStr);
void strReplaceChar(char *str, char pattern, char replace);

int opMemEq(const char* l, const char* r, size_t len);
char* opMemDup(const char* p, size_t len);
char* opStrDup(const char* str);
wchar_t* opStrDup(const wchar_t* str);
int opStrCmp(const wchar_t *l, const wchar_t *r);
int opStrCmpNoCase(const wchar_t *l, const wchar_t *r);
int opStrLen(const wchar_t *str);
int opMkDir(const char *path);
int opUnlink(const char *path);
int opRmDir(const char *path);

int doesFileExist(const char *filePath);

int readLinesFromFile(const char *filePath, list<string> &lines);
void splitTokenString(const char *str, char splitCh, vector<string> &out);
string packTokenString(vector<string> tokens, const char *splitCh);
int findIdxOfToken(const vector<string> &tokens, const char *name);

time_t str2Time(const char *str);
string time2Str(time_t t);

string correctQueryKeyName(const char *str);
int isRightQueryKeyName(const char *str);

int isStrContainKeys(const char *str, const char *keys);
