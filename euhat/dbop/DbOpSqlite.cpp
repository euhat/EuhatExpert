#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include "DbOpSqlite.h"
#include <sqlite/sqlite3euhat.h>
#include <../config.h>
#include <EuhatPostDef.h>

DbOpSqlite::DbOpSqlite()
{
	db_ = NULL;
}

DbOpSqlite::~DbOpSqlite()
{
	if (NULL != db_)
	{
		closeDb();
	}
}

int DbOpSqlite::openFileDb(const char *dbPath)
{
	int status = -1;
	sqlite3 *db = NULL;

	status = sqlite3_open(dbPath, &db);

	if (status != SQLITE_OK)
	{
		DBG(("open database error:%s.\n", sqlite3_errmsg(db)));
	}
	db_ = db;
	return 1;
}

static string encryptName(const char *str)
{
	char buf[1024];
	int i = 0;
	for (const char *p = str; *p != 0; p++, i++)
	{
		char c = *p;
		c *= i;
		c += i;
		if (c == 0)
			c = (char)173;
		buf[i] = c;
	}
	buf[i] = 0;
	return buf;
}

int DbOpSqlite::openFileDbEncrypted(const char *dbPath)
{
	string key = DB_KEY_3;
	key += DB_KEY_1;
	key += DB_KEY_4;
	key += DB_KEY_2;
	key = encryptName(key.c_str());

	if (!doesFileExist(dbPath))
	{
		if (openFileDb(dbPath))
		{
			sqlite3_rekey(db_, key.c_str(), (int)key.length());
			closeDb();
		}
		else
			return 0;
	}

	int ret = openFileDb(dbPath);
	if (ret)
	{
		sqlite3_key(db_, key.c_str(), (int)key.length());
	}
	return ret;
}

int DbOpSqlite::closeDb()
{
	if (NULL == db_)
		return 0;

	int status = -1;
	status = sqlite3_close(db_);

	if (status != SQLITE_OK)
		DBG(("close database error.\n"));

	db_ = NULL;
	return status;
}

string DbOpSqlite::selectString(const char *sql)
{
	int result = 0;
	sqlite3_stmt *stmt = NULL;
	string out;

	result = sqlite3_prepare_v2(db_, sql, -1, &stmt, NULL);

	if (result != SQLITE_OK)
	{
		// DBG(("error %s(%d): result:%d sql:%s!\n", __FUNCTION__, __LINE__, result, sql));
		isSucceeded_ = 0;
		return "";
	}
	else
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			char *str = (char *)sqlite3_column_text(stmt, 0);
			if (str == NULL)
				return "";
			out = str;
			break;
		}
	}

	if (stmt)
		sqlite3_finalize(stmt);

	isSucceeded_ = 1;
	return out;
}

int DbOpSqlite::selectCount(const char *sql)
{
	string out = selectString(sql);
	if (!isSucceeded_)
		return -1;
	return atoi(out.c_str());
}

int64_t DbOpSqlite::selectCount64(const char *sql)
{
	int result = 0;
	sqlite3_stmt *stmt = NULL;
	int64_t count = 0;

	result = sqlite3_prepare_v2(db_, sql, -1, &stmt, NULL);

	if (result != SQLITE_OK)
	{
		//DBG(("error %s(%d): result:%d sql:%s!\n", __FUNCTION__, __LINE__, result, sql));
		isSucceeded_ = 0;
		return -1;
	}
	else
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
			count = sqlite3_column_int64(stmt, 0);
	}

	if (stmt)
		sqlite3_finalize(stmt);

	isSucceeded_ = 1;
	return count;
}

int DbOpSqlite::insert(const char *sql)
{
	int  result = 0;
	char *pErrMsg = NULL;

	result = sqlite3_exec(db_, sql, NULL, NULL, &pErrMsg);

	if (result != SQLITE_OK)
	{
		DBG(("error %s(%d): result:%d error:%s sql:%s!\n", __FUNCTION__, __LINE__, result, pErrMsg, sql));
		return 0;
	}

	return 1;
}
