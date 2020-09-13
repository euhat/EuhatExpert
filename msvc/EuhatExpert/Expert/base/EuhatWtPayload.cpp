#include "stdafx.h"
#include <common/OpCommon.h>
#include "EuhatWtPayload.h"
#include "EuhatBase.h"
#include <dbop/DbOpSqliteTable.h>
#include <EuhatPostDefMfc.h>

#define CELL_TO_MEMBER_LOOP \
	CELL_TO_MEMBER(rowid, DbOpColumnTypeInt64) \
	CELL_TO_MEMBER(date, DbOpColumnTypeText) \
	CELL_TO_MEMBER(dat, DbOpColumnTypeBlob)

WorkTraceTable::WorkTraceTable(DbOpSqlite *db)
{
	tab_.reset(new DbOpSqliteTable());
	tab_->db_ = db;
	tab_->tableName_ = "WorkTrace";
#define CELL_TO_MEMBER(_name, _type) tab_->cols_.push_back({ #_name, _type });
	CELL_TO_MEMBER_LOOP
#undef CELL_TO_MEMBER

	tab_->create();
}

EuhatWtPayload::EuhatWtPayload(EuhatBase *euhatBase)
	: tab_(euhatBase->db_.get())
{
	euhatBase_ = euhatBase;
	reset();
}

EuhatWtPayload::~EuhatWtPayload()
{
}

void EuhatWtPayload::reset()
{
	for (int i = 0; i < EUHAT_WT_PAYLOAD_COUNT; i++) {
		payload_[i].mouse_ = EUHAT_WT_PL_NO_RECORD;
		payload_[i].keyboard_ = EUHAT_WT_PL_NO_RECORD;
	}
}

int EuhatWtPayload::read(const char *filePath)
{
	WhMutexGuard guard(&euhatBase_->mutexWtPayload_);

	string where = string("where date = '") + filePath + "'";
	list<vector<DbOpCell>> rows;
	tab_.tab_->select(where.c_str(), rows);
	if (rows.size() == 0)
	{
		reset();
		return 0;
	}
	vector<DbOpCell> &row = rows.front();
	if (sizeof(payload_) != row[2].i_)
	{
		reset();
		return 0;
	}
	memcpy(payload_, row[2].buf_.get(), sizeof(payload_));
	return 1;
}

int EuhatWtPayload::write(const char *filePath)
{
	WhMutexGuard guard(&euhatBase_->mutexWtPayload_);

	string where = string("where date = '") + filePath + "'";
	list<vector<DbOpCell>> rows;
	tab_.tab_->select(where.c_str(), rows);
	vector<DbOpCell> row;
	row.push_back({ NULL, 0 });
	row.push_back(DbOpCell()); row.back().buf_.reset(opStrDup(filePath));
	row.push_back(DbOpCell()); row.back().buf_.reset((char *)payload_); row.back().i_ = sizeof(payload_);
	if (rows.size() == 0)
		tab_.tab_->insert(row);
	else
		tab_.tab_->update(where.c_str(), row);
	row.back().buf_.release();

	return 1;
}

void EuhatWtPayload::setValue(int idx, int type, int val)
{
	if (type == EUHAT_WT_PL_TYPE_MOUSE)
		payload_[idx].mouse_ = val;
	else if (type == EUHAT_WT_PL_TYPE_KEYBOARD)
		payload_[idx].keyboard_ = val;
}

int EuhatWtPayload::getValue(int idx, int type)
{
	if (type == EUHAT_WT_PL_TYPE_MOUSE)
		return payload_[idx].mouse_;
	else if (type == EUHAT_WT_PL_TYPE_KEYBOARD)
		return payload_[idx].keyboard_;
	return EUHAT_WT_PL_NO_RECORD;
}