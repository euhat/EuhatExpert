#pragma once

#include "../../base/EuhatWtPayload.h"

class EuhatBase;

class WorkTrace 
{
	void updateNoMoveSlot(int slot, int value = EUHAT_WT_PL_NO_MOVE);
	void updateNoMoveGap(int curSlot);

	EuhatWtPayload payload_;
	int lastSlot_;

public:
	WorkTrace(EuhatBase *euhatBase);
	~WorkTrace();

	int startRecord();
	int stopRecord();

	int recordMouse(WPARAM wParam, LPARAM lParam);
	int recordKeyboard(WPARAM wParam, LPARAM lParam);
	void payloadAddOne(int type);

	EuhatBase *euhatBase_;
	HANDLE threadHandle_;
};