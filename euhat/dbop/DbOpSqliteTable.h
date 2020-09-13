#pragma once

#include <common/OpCommon.h>
#include "DbOpSqlite.h"

#define DbOpColumnTypeText		0
#define DbOpColumnTypeInt		1
#define DbOpColumnTypeInt64		2
#define DbOpColumnTypeBlob		3

struct DbOpColumn
{
	string name_;
	int type_;
};

struct DbOpCell
{
	unique_ptr<char[]> buf_;
	int64_t i_;
};

class DbOpSqliteTable
{
	int exePrepare(const char *sql, vector<DbOpCell> &cells);
public:
	int create();
	int insert(vector<DbOpCell> &cells);
	int select(const char *where, list<vector<DbOpCell>> &rows);
	int update(const char *where, vector<DbOpCell> &cells);
	string tableName_;
	vector<DbOpColumn> cols_;
	DbOpSqlite *db_;
};