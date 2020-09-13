#include <EuhatPreDef.h>
#include "FileManCmdTable.h"
#include <EuhatPostDef.h>
#include <dbop/DbOpSqlite.h>
#include <dbop/DbOpSqliteTable.h>
#include <common/WhCommon.h>

#define FILE_MAN_CMD_TABLE_KEY_COL 0

FileManCmd::FileManCmd()
	: FileManCmd(FmCmdNone, "", "")
{

}

FileManCmd::FileManCmd(FileManCmdType type, const char *localPath, const char *peerPath)
{
	id_ = -1;
	type_ = type;
	localPath_ = localPath;
	peerPath_ = peerPath;
	offset_ = 0;
	totalFileSize_ = -1;
	state_ = FmResultNotStart;
}

#define CELL_TO_MEMBER_LOOP \
	CELL_TO_MEMBER(rowid, DbOpColumnTypeInt64) \
	CELL_TO_MEMBER(type, DbOpColumnTypeInt) \
	CELL_TO_MEMBER(localPath, DbOpColumnTypeText) \
	CELL_TO_MEMBER(peerPath, DbOpColumnTypeText) \
	CELL_TO_MEMBER(offset, DbOpColumnTypeInt64) \
	CELL_TO_MEMBER(totalFileSize, DbOpColumnTypeInt64) \
	CELL_TO_MEMBER(state, DbOpColumnTypeInt)

FileManCmdTable::FileManCmdTable(DbOpSqlite *db)
{
	tab_.reset(new DbOpSqliteTable());
	tab_->db_ = db;
	tab_->tableName_ = "FileManCmd";
#define CELL_TO_MEMBER(_name, _type) tab_->cols_.push_back({ #_name, _type });
	CELL_TO_MEMBER_LOOP
#undef CELL_TO_MEMBER
	
	tab_->create();
	string cmd = "delete from " + tab_->tableName_;
	tab_->db_->insert(cmd.c_str());
}

FileManCmdTable::~FileManCmdTable()
{

}

void FileManCmdTable::rowToMember(FileManCmd &cmd, vector<string> &row)
{
	int i = 0;
	
	cmd.id_ = whStrToInt64(row[i++].c_str());
	cmd.type_ = (FileManCmdType)atoi(row[i++].c_str());
	cmd.localPath_ = row[i++];
	cmd.peerPath_ = row[i++];
	cmd.offset_ = whStrToInt64(row[i++].c_str());
	cmd.totalFileSize_ = whStrToInt64(row[i++].c_str());
	cmd.state_ = (FileManResultType)atoi(row[i++].c_str());
}

void FileManCmdTable::memberToRow(vector<string> &row, FileManCmd &cmd)
{
	row.push_back(int64ToString(cmd.id_));
	row.push_back(intToString(cmd.type_));
	row.push_back(cmd.localPath_);
	row.push_back(cmd.peerPath_);
	row.push_back(int64ToString(cmd.offset_));
	row.push_back(int64ToString(cmd.totalFileSize_));
	row.push_back(intToString(cmd.state_));
}

int FileManCmdTable::append(FileManCmd &cmd)
{
	vector<string> row;
	memberToRow(row, cmd);
	tab_->insert(row);
	return 1;
}

int FileManCmdTable::update(FileManCmd &cmd)
{
	vector<string> row;
	memberToRow(row, cmd);
	tab_->update(FILE_MAN_CMD_TABLE_KEY_COL, row);
	return 1;
}

int FileManCmdTable::del(int64_t id)
{
	string sql = "delete from " + tab_->tableName_ + " where rowid = " + int64ToString(id) + ";";
	tab_->db_->insert(sql.c_str());
	return 1;
}

int FileManCmdTable::find(int64_t id, FileManCmd &cmd)
{
	list<vector<string>> rows;
	string where = "where rowid = " + int64ToString(id);
	tab_->select(where.c_str(), rows);
	if (rows.size() < 1)
		return 0;

	vector<string> &row = rows.front();
	rowToMember(cmd, row);
	return 1;
}

int FileManCmdTable::getFirst(FileManCmd &cmd)
{
	list<vector<string>> rows;
	string where = "order by rowid limit 1";
	tab_->select(where.c_str(), rows);
	if (rows.size() < 1)
		return 0;

	vector<string> &row = rows.front();
	rowToMember(cmd, row);
	return 1;
}