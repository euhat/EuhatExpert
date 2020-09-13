#pragma once

#include <common/OpCommon.h>

struct sqlite3;

class DbOpSqlite
{
public:
	DbOpSqlite();
	~DbOpSqlite();

	int openFileDb(const char *dbPath);
	int openFileDbEncrypted(const char *dbPath);
	int closeDb();

	string selectString(const char *sql);
	int selectCount(const char *sql);
	int64_t selectCount64(const char *sql);

	int insert(const char *sql);

	sqlite3 *db_;
	int isSucceeded_;
};