#pragma once

#include <common/OpCommon.h>
#include "../common/FileManType.h"

class DbOpSqlite;
class DbOpSqliteTable;

struct FileManCmd
{
	FileManCmd();
	FileManCmd(FileManCmdType type, const char *localPath, const char *peerPath);

	int64_t id_;
	FileManCmdType type_;
	string localPath_;
	string peerPath_;
	int64_t offset_;
	int64_t totalFileSize_;
	FileManResultType state_;
};

class FileManCmdTable
{
	void rowToMember(FileManCmd &cmd, vector<string> &row);
	void memberToRow(vector<string> &row, FileManCmd &cmd);

	unique_ptr<DbOpSqliteTable> tab_;

public:
	FileManCmdTable(DbOpSqlite *db);
	~FileManCmdTable();

	int find(int64_t id, FileManCmd &cmd);
	int getFirst(FileManCmd &cmd);
	int append(FileManCmd &cmd);
	int update(FileManCmd &cmd);
	int del(int64_t id);
};