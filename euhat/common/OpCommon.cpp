#include <EuhatPreDef.h>
#include <time.h>
#include "OpCommon.h"
#include <string.h>
#include <EuhatPostDef.h>

int memToFile(const char *path, const char *pData, int iLen)
{
	opUnlink(path);

	FILE *fp = fopen(path, "wb+");
	if (NULL == fp)
		return 0;

	fwrite(pData, 1, iLen, fp);

	fclose(fp);
	return 1;
}

int memFromFile(const char *path, char *pData, int iLen)
{
	FILE *fp = fopen(path, "rb");
	if (NULL == fp)
		return 0;

	fread(pData, 1, iLen, fp);

	fclose(fp);
	return 1;
}

char *memFromWholeFile(const char *path, unsigned int *len)
{
	FILE *fp = fopen(path, "rb");
	if (NULL == fp)
		return NULL;

	fseek(fp, 0, SEEK_END);
	*len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *pData = (char *)malloc(*len + 1);
	fread(pData, 1, *len, fp);
	pData[*len] = 0;

	fclose(fp);
	return pData;
}

int loadKVMapFromFile(const char *filePath, map<string, string> &kvMap)
{
	FILE *fp = fopen(filePath, "r");
	if (NULL == fp)
		return 0;

	char buf[1024];
	while (fgets(buf, 1023, fp) != NULL)
	{
		char *eq = strstr(buf, "=");
		if (NULL == eq)
			continue;
		*eq++ = 0;

		char *retLine = strstr(eq, "\n");
		if (NULL != retLine)
			retLine[0] = 0;

		kvMap[buf] = eq;
	}

	fclose(fp);
	return 1;
}

int saveKVMapToFile(const char *filePath, map<string, string> &kvMap)
{
	FILE *fp = fopen(filePath, "w+");
	if (NULL == fp)
		return 0;

	for (map<string, string>::iterator it = kvMap.begin(); it != kvMap.end(); it++)
	{
		fprintf(fp, "%s,%s\n", it->first.c_str(), it->second.c_str());
	}

	fclose(fp);
	return 1;
}

int readIniInt(const char *iniFileName, const char *sectionName, const char *name, int defaultVal)
{
	return atoi(readIniString(iniFileName, sectionName, name, intToString(defaultVal).c_str()).c_str());
}

int writeIniInt(const char *iniFileName, const char *sectionName, const char *name, int value)
{
	return writeIniString(iniFileName, sectionName, name, intToString(value).c_str());
}

string intToString(int i)
{
	char buf[64];
	sprintf(buf, "%d", i);
	return buf;
}

string int64ToString(int64_t i)
{
	char buf[64];
	sprintf(buf, "%lld", i);
	return buf;
}

string strToLower(const char *str)
{
	char *tmp = opStrDup(str);

	for (char *p = tmp; *p != 0; p++)
	{
		if (*p >= 'A' && *p <= 'Z')
			*p = (*p) | 0x20;
	}

	string out = tmp;

	free(tmp);

	return out;
}

int strIsEndWith(const char *str, const char *endStr)
{
	size_t strLen = strlen(str);
	size_t endStrLen = strlen(endStr);
	if (strLen < endStrLen)
		return 0;
	const char *pStr = str + strLen - endStrLen;
	if (0 == strcmp(pStr, endStr))
		return 1;
	return 0;
}

int strIsStartWith(const char *str, const char *startStr)
{
	size_t len = strlen(startStr);
	if (strlen(str) < len || memcmp(str, startStr, len) != 0)
		return 0;
	return 1;
}

void strReplaceChar(char *str, char pattern, char replace)
{
	for (char *p = str; *p != 0; p++)
	{
		if (*p == pattern)
			*p = replace;
	}
}

int readLinesFromFile(const char *filePath, list<string> &lines)
{
	FILE *fp = fopen(filePath, "r");
	if (NULL == fp)
		return 0;

	char buf[2048];
	while (fgets(buf, 2048 - 1, fp) != NULL)
	{
		char *retLine = strstr(buf, "\n");
		if (NULL != retLine)
			retLine[0] = 0;

		lines.push_back(buf);
	}

	fclose(fp);
	return 1;
}

void splitTokenString(const char *str, char splitCh, vector<string> &out)
{
	if (NULL == str)
		return;

	char *newStr = opStrDup(str);

	char *pStart = newStr;
	char *pEnd = newStr;

	for (; ; pEnd++)
	{
		if (*pEnd == splitCh)
		{
			*pEnd = 0;
			out.push_back(pStart);
			pStart = pEnd + 1;
		}
		else if (*pEnd == 0)
		{
			out.push_back(pStart);
			break;
		}
	}

	free(newStr);
}

string packTokenString(vector<string> tokens, const char *splitCh)
{
	string out;
	for (vector<string>::iterator it = tokens.begin(); it != tokens.end(); it++)
	{
		if (it != tokens.begin())
			out += splitCh;
		out += it->c_str();
	}
	return out;
}

int findIdxOfToken(const vector<string> &tokens, const char *name)
{
	int i = 0;
	for (auto it = tokens.begin(); it != tokens.end(); it++, i++)
	{
		if (*it == name)
			return i;
	}
	return -1;
}

time_t str2Time(const char *str)
{
	if (str == NULL)
	{
		return 0;
	}

	if (strlen(str) < 19)
	{
		return 0;
	}

	//2030-01-01 01:00:00
	int y1 = 0, y2 = 0, y3 = 0, y4 = 0;
	int m1 = 0, m2 = 0, d1 = 0, d2 = 0;
	int h1 = 0, h2 = 0, M1 = 0, M2 = 0, S1 = 0, S2 = 0;

	y1 = *(str + 0) - '0';
	y2 = *(str + 1) - '0';
	y3 = *(str + 2) - '0';
	y4 = *(str + 3) - '0';
	m1 = *(str + 5) - '0';
	m2 = *(str + 6) - '0';
	d1 = *(str + 8) - '0';
	d2 = *(str + 9) - '0';
	h1 = *(str + 11) - '0';
	h2 = *(str + 12) - '0';
	M1 = *(str + 14) - '0';
	M2 = *(str + 15) - '0';
	S1 = *(str + 17) - '0';
	S2 = *(str + 18) - '0';

	int y = y1 * 1000 + y2 * 100 + y3 * 10 + y4;
	int m = m1 * 10 + m2;
	int d = d1 * 10 + d2;
	int h = h1 * 10 + h2;
	int M = M1 * 10 + M2;
	int S = S1 * 10 + S2;

	struct tm t;
	time_t t_of_day;
	t.tm_year = y - 1900;
	t.tm_mon = m - 1;
	t.tm_mday = d;
	t.tm_hour = h;
	t.tm_min = M;
	t.tm_sec = S;
	t_of_day = mktime(&t);

	return t_of_day;
}

string time2Str(time_t t)
{
	struct tm *localTime = localtime(&t);
	char buf[64];
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
		localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
		localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
	return buf;
}

string correctQueryKeyName(const char *str)
{
	if (NULL == str)
		return "";

	string out;
	for (const char *p = str; *p != 0; p++)
	{
		if (*p == '_')
			out += *p;
		else if ('0' <= *p && *p <= '9')
			out += *p;
		else if ('A' <= *p && *p <= 'Z')
			out += *p;
		else if ('a' <= *p && *p <= 'z')
			out += *p;
		else if ((const unsigned char)*p > 0x80)
		{
			if (*(p + 1) != 0 && (const unsigned char)*(p + 1) > 0x80)
			{
				out += *p;
				p++;
				out += *p;
			}
		}
	}
	return out;
}

int isRightQueryKeyName(const char *str)
{
	if (NULL == str)
		return 0;

	for (const char *p = str; *p != 0; p++)
	{
		if (*p == '_')
		{
		}
		else if ('0' <= *p && *p <= '9')
		{
		}
		else if ('A' <= *p && *p <= 'Z')
		{
		}
		else if ('a' <= *p && *p <= 'z')
		{
		}
		else if ((const unsigned char)*p > 0x80)
		{
			if (*(p + 1) != 0 && (const unsigned char)*(p + 1) > 0x80)
			{

			}
			else
				return 0;
		}
		else
			return 0;
	}
	return 1;
}

int isStrContainKeys(const char *str, const char *keys)
{
	for (const char *p = keys; *p != 0; p++)
	{
		if (NULL != strchr(str, *p))
			return 1;
	}
	return 0;
}

int opStrLen(const wchar_t *str)
{
	int i = 0;
	for (const wchar_t *p = str; *p != 0; p++)
		i++;
	return i;
}

wchar_t *opStrDup(const wchar_t *str)
{
	int size = (opStrLen(str) + 1) * sizeof(wchar_t);
	wchar_t *p = (wchar_t *)malloc(size);
	memcpy(p, str, size);
	return p;
}

int opStrCmp(const wchar_t *l, const wchar_t *r)
{
	for (; *l != 0 && *l == *r; l++, r++)
		;
	return *l - *r;
}

int opStrCmpNoCase(const wchar_t *l, const wchar_t *r)
{
	for (; *l != 0; l++, r++)
	{
		wchar_t lc = *l;
		if ('A' <= lc && lc <= 'Z')
			lc |= 0x20;
		wchar_t rc = *r;
		if ('A' <= rc && rc <= 'Z')
			rc |= 0x20;
		if (lc != rc)
			return lc - rc;
	}
	return *r == 0? 0 : -1;
}