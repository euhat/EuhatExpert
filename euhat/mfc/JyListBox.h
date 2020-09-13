#pragma once

#include <common/OpCommon.h>
#include <EuhatChildWnd.h>

class JyListBox : public CListBox
{
public:
	struct Item
	{
		CString str_;
		void *userData_;
	};
	JyListBox();

	void insertStr(int iIndex, CString str, void *userData = NULL);
	Item *getItem(int idx);

	virtual void ResetContent();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

	unique_ptr<EuhatFont> font_;
};