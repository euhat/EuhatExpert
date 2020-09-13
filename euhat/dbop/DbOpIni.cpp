#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/JyBuf.h>
#include "DbOpIni.h"
#include "DbOpSqliteTable.h"
#include <common/JyBigNum.h>
#include <EuhatPostDef.h>

#define CELL_TO_MEMBER_LOOP \
	CELL_TO_MEMBER(rowid, DbOpColumnTypeInt64) \
	CELL_TO_MEMBER(section, DbOpColumnTypeText) \
	CELL_TO_MEMBER(key, DbOpColumnTypeText) \
	CELL_TO_MEMBER(val, DbOpColumnTypeText)

DbOpIni::DbOpIni(DbOpSqlite *db)
{
	tab_.reset(new DbOpSqliteTable());
	tab_->db_ = db;
	tab_->tableName_ = DEFAULT_SQLITE_INI_TABLE_NAME;
#define CELL_TO_MEMBER(_name, _type) tab_->cols_.push_back({ #_name, _type });
	CELL_TO_MEMBER_LOOP
#undef CELL_TO_MEMBER

	tab_->create();

	string sql = "create index IF NOT EXISTS " + tab_->tableName_ + "_" + "section" + "_INDEX on " + tab_->tableName_ + "(" + "section" + ");";
	tab_->db_->insert(sql.c_str());
	sql = "create index IF NOT EXISTS " + tab_->tableName_ + "_" + "key" + "_INDEX on " + tab_->tableName_ + "(" + "key" + ");";
	tab_->db_->insert(sql.c_str());
}

void DbOpIni::write(const char *section, const char *key, const char *val)
{
	string sql = "delete from " + tab_->tableName_ + " where section = '" + string(section) + "' and key = '" + key + "'";
	tab_->db_->insert(sql.c_str());
	vector<DbOpCell> row;
	row.push_back({ NULL, 0 });
	row.push_back(DbOpCell()); row.back().buf_.reset(opStrDup(section));
	row.push_back(DbOpCell()); row.back().buf_.reset(opStrDup(key));
	row.push_back(DbOpCell()); row.back().buf_.reset(opStrDup(val));
	tab_->insert(row);
}

void DbOpIni::write(const char *section, const char *key, int val)
{
	write(section, key, intToString(val).c_str());
}

string DbOpIni::readStr(const char *section, const char *key, const char *defaultVal)
{
	list<vector<DbOpCell>> rows;
	string where = "where section = '" + string(section) + "' and key = '" + key + "'";
	tab_->select(where.c_str(), rows);
	if (rows.size() < 1)
		return defaultVal;
	vector<DbOpCell> &row = rows.front();
	return row[3].buf_.get();
}

int DbOpIni::readInt(const char *section, const char *key, int defaultVal)
{
	string str = readStr(section, key, intToString(defaultVal).c_str());
	return atoi(str.c_str());
}

void DbOpIni::readBuf(const char *section, const char *keyName, JyBuf &buf)
{
#if 0
	string base64Str = readStr(section, keyName);

	int len = Base64decode_len(base64Str.c_str());
	buf.reset(len);
	Base64decode(buf.data_.get(), base64Str.c_str());
	buf.size_--;
#else
	string hexStr = readStr(section, keyName);
	JyBigNum::hex2data(buf, hexStr.c_str());
#endif
}

void DbOpIni::writeBuf(const char *section, const char *keyName, JyBuf &buf)
{
#if 0
	int encodedLen = Base64encode_len(buf.size_);
	JyBuf bufEncoded(encodedLen);
	Base64encode(bufEncoded.data_.get(), buf.data_.get(), buf.size_);

	write(section, keyName, bufEncoded.data_.get());
#else
	string hexStr = JyBigNum::data2hex((unsigned char *)buf.data_.get(), buf.size_);
	write(section, keyName, hexStr.c_str());
#endif
}