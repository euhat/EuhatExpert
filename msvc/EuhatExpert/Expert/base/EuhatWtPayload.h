#pragma once

#include <common/OpCommon.h>

#define EUHAT_WT_PAYLOAD_COUNT (60 * 24)

#define EUHAT_WT_PL_TYPE_MOUSE 0
#define EUHAT_WT_PL_TYPE_KEYBOARD 1

#define EUHAT_WT_PL_NO_MOVE -1
#define EUHAT_WT_PL_HANG_UP -2
#define EUHAT_WT_PL_NO_RECORD -3

#define EUHAT_WT_PL_FILE_PATH_PATTERN "%Y%m%d"

struct EuhatWtPl
{
	int mouse_;
	int keyboard_;
};

class EuhatBase;
class DbOpSqlite;
class DbOpSqliteTable;

class WorkTraceTable
{
public:
	WorkTraceTable(DbOpSqlite *db);

	unique_ptr<DbOpSqliteTable> tab_;
};

class EuhatWtPayload
{
	EuhatWtPl payload_[EUHAT_WT_PAYLOAD_COUNT];
	EuhatBase *euhatBase_;
	WorkTraceTable tab_;

public:
	EuhatWtPayload(EuhatBase *euhatBase);
	~EuhatWtPayload();

	void reset();
	int read(const char *filePath);
	int write(const char *filePath);

	void setValue(int idx, int type, int val);
	int getValue(int idx, int type);
};