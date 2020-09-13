#include <EuhatPreDef.h>
#include "JyWorkThread.h"
#include <EuhatPostDef.h>

WH_THREAD_DEF(JyWorkThread::workThreadCb, arg)
{
	WH_THREAD_PREPROCESS;

	JyWorkThread *pThis = (JyWorkThread *)arg;
	pThis->workProc();

	return 0;
}

int JyWorkThread::workProc()
{
	return 1;
}

JyWorkThread::JyWorkThread()
{
	workThreadIsRunning_ = 0;
	workThreadHandle_ = WH_INVALID_THREAD;
}

JyWorkThread::~JyWorkThread()
{
	stop();
}

int JyWorkThread::start()
{
	workThreadIsRunning_ = 1;
	whThreadCreate(workThreadHandle_, workThreadCb, this);
	return 1;
}

int JyWorkThread::stop()
{
	if (workThreadHandle_ == WH_INVALID_THREAD)
		return 0;

	workThreadIsRunning_ = 0;
	whThreadJoin(workThreadHandle_);
	workThreadHandle_ = WH_INVALID_THREAD;

	return 1;
}