#pragma once

#include <common/OpCommon.h>

class WorkSpan
{
public:
	int beginSecs_;
	string startTime_;
	string endTime_;
	int untilSecs_;

	int load(vector<string> &cells);
	int save(FILE *fp, int split);
};