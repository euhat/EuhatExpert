#pragma once

#include <common/OpCommon.h>
#include <dbop/DbOpSqliteTable.h>

class JyBuf;

class DbOpIni
{
public:
	DbOpIni(DbOpSqlite *db);

	void write(const char *section, const char *key, const char *val);
	void write(const char *section, const char *key, int val);

	string readStr(const char *section, const char *key, const char *defaultVal = "");
	int readInt(const char *section, const char *key, int defaultVal = 0);

	void readBuf(const char *section, const char *keyName, JyBuf &buf);
	void writeBuf(const char *section, const char *keyName, JyBuf &buf);

	unique_ptr<DbOpSqliteTable> tab_;
};