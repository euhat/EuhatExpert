#pragma once

#include <common/OpCommon.h>

class EuhatPath
{
public:
	EuhatPath();

	void goSub(const char *fileName);
	void goUp();

	void inStr(const char *path);
	string toStr(int isAbsoluteDir = 1);

	list<string> path_;
	int isUnix_;
};