#pragma once

#include <common/OpCommon.h>

class JyListCtrl : public CListCtrl
{
	static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

protected:
	DECLARE_MESSAGE_MAP()

public:
	enum ColumnType
	{
		ColumnTypeText,
		ColumnTypeInt,
		ColumnTypeDateTime
	};
	class Column
	{
	public:
		Column()
		{
			type_ = ColumnTypeText;
			isDesc_ = 0;
		}

		int isDesc_;
		ColumnType type_;
	};

	void init();
	void sort();

	afx_msg void OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult);

	vector<Column> vecCol_;
	int colToSort_;
};