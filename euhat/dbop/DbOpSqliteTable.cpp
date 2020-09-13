#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include "DbOpSqliteTable.h"
#include <sqlite/sqlite3euhat.h>
#include <EuhatPostDef.h>

int DbOpSqliteTable::create()
{
	ostringstream ss;
	int isFirstCol = 1;
	ss << "CREATE TABLE IF NOT EXISTS " << tableName_ << "(";
	for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++)
	{
		if (it->name_ == "rowid")
			continue;

		if (isFirstCol)
			isFirstCol = 0;
		else
			ss << ", ";
		ss << "[" << it->name_ << "] ";
		if (it->type_ == DbOpColumnTypeText)
			ss << "TEXT";
		else if (it->type_ == DbOpColumnTypeBlob)
			ss << "BLOB";
		else
			ss << "INTEGER";
	}
	ss << ");";
	string sql = ss.str();

	int	result = 0;
	char *pErrMsg = NULL;

	result = sqlite3_exec(db_->db_, sql.c_str(), NULL, NULL, &pErrMsg);

	if (result != SQLITE_OK)
	{
		DBG(("error %s(%d): result:%d error:%s sql:%s!\n", __FUNCTION__, __LINE__, result, pErrMsg, sql.c_str()));
		return 0;
	}
	return 1;
}

class DbOpSqliteStmt
{
public:
	DbOpSqliteStmt() { stmt_ = NULL; }
	~DbOpSqliteStmt() { if (NULL != stmt_) sqlite3_finalize(stmt_); }

	sqlite3_stmt *stmt_;
};

int DbOpSqliteTable::exePrepare(const char *sql, vector<DbOpCell> &cells)
{
	DbOpSqliteStmt stmt;
	int res = sqlite3_prepare_v2(db_->db_, sql, strlen(sql), &stmt.stmt_, 0);
	if (res != SQLITE_OK)
	{
		DBG(("sqlite prepare error [%s].\n", sqlite3_errmsg(db_->db_)));
		return 0;
	}

	int i = 0;
	int stmtIdx = 1;
	for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++, i++)
	{
		if (it->name_ == "rowid")
			continue;

		if (it->type_ == DbOpColumnTypeText)
			sqlite3_bind_text(stmt.stmt_, stmtIdx, cells[i].buf_.get(), strlen(cells[i].buf_.get()), NULL);
		else if (it->type_ == DbOpColumnTypeBlob)
			sqlite3_bind_blob(stmt.stmt_, stmtIdx, cells[i].buf_.get(), (int)cells[i].i_, NULL);
		else if (it->type_ == DbOpColumnTypeInt)
			sqlite3_bind_int(stmt.stmt_, stmtIdx, (int)cells[i].i_);
		else
			sqlite3_bind_int64(stmt.stmt_, stmtIdx, cells[i].i_);
		stmtIdx++;
	}

	res = sqlite3_step(stmt.stmt_);
	if (res != SQLITE_DONE)
	{
		DBG(("sqlite step error.\n"));
		return 0;
	}
	return 1;
}

int DbOpSqliteTable::insert(vector<DbOpCell> &cells)
{
	ostringstream ss;
	int isFirstCol = 1;
	ss << "insert into " << tableName_ << "(";
	for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++)
	{
		if (it->name_ == "rowid")
			continue;

		if (isFirstCol)
			isFirstCol = 0;
		else
			ss << ", ";
		ss << it->name_;
	}
	ss << ") values (";
	isFirstCol = 1;
	for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++)
	{
		if (it->name_ == "rowid")
			continue;

		if (isFirstCol)
			isFirstCol = 0;
		else
			ss << ", ";
		ss << "?";
	}
	ss << ");";
	string sql = ss.str();

	return exePrepare(sql.c_str(), cells);
}

int DbOpSqliteTable::select(const char *where, list<vector<DbOpCell>> &rows)
{
	ostringstream ss;
	ss << "select ";
	int isFirstCol = 1;
	for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++)
	{
		if (isFirstCol)
			isFirstCol = 0;
		else
			ss << ", ";
		ss << it->name_;
	}
	ss << " from " << tableName_ << " " << where;
	string sql = ss.str();

	DbOpSqliteStmt stmt;
	int res = sqlite3_prepare_v2(db_->db_, sql.c_str(), sql.length(), &stmt.stmt_, 0);
	if (res != SQLITE_OK)
	{
		DBG(("sqlite prepare error [%s].\n", sqlite3_errmsg(db_->db_)));
		return 0;
	}

	while ((res = sqlite3_step(stmt.stmt_)) == SQLITE_ROW)
	{
		rows.push_back(vector<DbOpCell>());
		vector<DbOpCell> &row = rows.back();

		int i = 0;
		for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++, i++)
		{
			row.push_back(DbOpCell());
			DbOpCell &cell = row.back();

			if (it->type_ == DbOpColumnTypeText)
				cell.buf_.reset(opStrDup((char *)sqlite3_column_text(stmt.stmt_, i)));
			else if (it->type_ == DbOpColumnTypeBlob)
			{
				char *mem = (char *)sqlite3_column_blob(stmt.stmt_, i);
				int size = sqlite3_column_bytes(stmt.stmt_, i);
				char *toCopy = (char *)malloc(size);
				memcpy(toCopy, mem, size);
				cell.buf_.reset(toCopy);
				cell.i_ = size;
			}
			else if (it->type_ == DbOpColumnTypeInt)
				cell.i_ = sqlite3_column_int(stmt.stmt_, i);
			else
				cell.i_ = sqlite3_column_int64(stmt.stmt_, i);
		}
	}

	return 1;
}

int DbOpSqliteTable::update(const char *where, vector<DbOpCell> &cells)
{
	ostringstream ss;
	ss << "update " << tableName_ << " set ";
	int i = 0;
	int isFirstCol = 1;
	for (vector<DbOpColumn>::iterator it = cols_.begin(); it != cols_.end(); it++, i++)
	{
		if (it->name_ == "rowid")
			continue;

		if (isFirstCol)
		{
			isFirstCol = 0;
		}
		else
			ss << ", ";
		ss << it->name_ << " = ?";
	}
	ss << " " << where;
	string sql = ss.str();

	return exePrepare(sql.c_str(), cells);
}