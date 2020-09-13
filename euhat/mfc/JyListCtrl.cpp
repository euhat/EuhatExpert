#include "stdafx.h"
#include "JyListCtrl.h"
#include <EuhatPostDefMfc.h>

BEGIN_MESSAGE_MAP(JyListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &JyListCtrl::OnColumnClick)
END_MESSAGE_MAP()

void JyListCtrl::init()
{
	vecCol_.resize(GetHeaderCtrl()->GetItemCount());
}

void JyListCtrl::sort()
{
	int rowCount = GetItemCount();
	for (int i = 0; i < rowCount; i++)
		SetItemData(i, i);

	SortItems(MyCompareProc, (DWORD_PTR)this);
}

void JyListCtrl::OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	colToSort_ = pNMLV->iSubItem;

	sort();

	vecCol_[colToSort_].isDesc_ = !vecCol_[colToSort_].isDesc_;

	*pResult = 0;
}

int CALLBACK JyListCtrl::MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    int rowL = (int)lParam1;
    int rowR = (int)lParam2;
	JyListCtrl *pThis = (JyListCtrl *)lParamSort;

    CString l = pThis->GetItemText(rowL, pThis->colToSort_);
    CString r = pThis->GetItemText(rowR, pThis->colToSort_);

	Column &col = pThis->vecCol_[pThis->colToSort_];
	if (col.type_ == ColumnTypeText || col.type_ == ColumnTypeDateTime)
	{
		if (col.isDesc_)
			return l.CompareNoCase(r);
		else
			return r.CompareNoCase(l);
	}
	else
	{
		string lStr = CT2A(l);
		string rStr = CT2A(r);
		int li = atoi(lStr.c_str());
		int ri = atoi(rStr.c_str());
		if (col.isDesc_)
			return li - ri;
		else
			return ri - li;
	}
	return 0;
}