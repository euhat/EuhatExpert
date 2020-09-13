#pragma once

#include <common/JyMsgLoop.h>
#include "FmTask.h"

class FmScheduler;

class FmLocal : public JyMsgLoop
{
public:
	FmLocal(FmScheduler *scheduler);
	~FmLocal();

	void onUserLoop(JyMsg &msg);
	void onGetSubList(FmTaskGetSubList *task);
	void onDelFile(FmTaskDel *task);
	void onNewFile(FmTaskNew *task);

	void onInit(JyMsg &msg);
	void onTimer(JyMsg &msg);
	virtual int onWork(JyMsg &msg);

	FmScheduler *scheduler_;
};