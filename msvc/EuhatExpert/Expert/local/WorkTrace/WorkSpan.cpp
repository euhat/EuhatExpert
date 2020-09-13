#include "stdafx.h"
#include "WorkSpan.h"
#include <EuhatPostDefMfc.h>

int WorkSpan::load(vector<string> &cells)
{
	if (cells.size() < 4)
	{
		DBG(("work span cell count is less than 2.\n"));
		return 0;
	}
	beginSecs_ = atoi(cells[0].c_str());
	startTime_ = cells[1];
	endTime_ = cells[2];
	untilSecs_ = atoi(cells[3].c_str());
	return 1;
}

int WorkSpan::save(FILE *fp, int split)
{
	fprintf(fp, "%d%c%s%c%s%c%d", beginSecs_, split, startTime_.c_str(), split, endTime_.c_str(), split, untilSecs_);
	return 1;
}