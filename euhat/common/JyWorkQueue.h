#pragma once

#include "JyWorkThread.h"
#include <common/JyMutex.h>

template<class T>
class JyWorkQueue : public JyWorkThread, public JyMutexWaitList<T>
{
	virtual int workProc()
	{
		T t;
		int got;
		while (workThreadIsRunning_)
		{
			got = this->popFrontWait(t);
			if (got && isUnique(t))
			{
				WhMutexGuard g(&this->mutex_);
				this->list_.erase(remove_if(this->list_.begin(), this->list_.end(), [this](T &t){ return isUnique(t); }), this->list_.end());
			}

			if (got)
			{
				onWork(t);
			}
		}
		do
		{
			got = this->popFront(t);

			if (got)
			{
				onWork(t);
			}
		} while (got);

		return 1;
	}

protected:
	virtual int onWork(T &t)
	{
		return 1;
	}

	virtual int isUnique(T &t)
	{
		return 0;
	}
	int stop()
	{
		workThreadIsRunning_ = 0;
		this->signal();
		JyWorkThread::stop();

		return 1;
	}

public:
	~JyWorkQueue()
	{
		stop();
	}
	int start()
	{
		return JyWorkThread::start();
	}
};