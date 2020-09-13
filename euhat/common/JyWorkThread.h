#pragma once

#include "OpCommon.h"
#include "WhCommon.h"

class JyWorkThread
{
	static WH_THREAD_DEF(workThreadCb, arg);

protected:
	virtual int workProc();

	int workThreadIsRunning_;
	WhThreadHandle workThreadHandle_;

public:
	JyWorkThread();
	~JyWorkThread();
	int start();
	int stop();
	int isRunning()
	{
		return workThreadIsRunning_;
	}
};