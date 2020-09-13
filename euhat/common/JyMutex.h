#pragma once

#include <common/OpCommon.h>
#include <common/WhCommon.h>

class WhMutex
{
public:
	WhMutex()
	{
		whMutexInit(&mutex_);
	}
	~WhMutex()
	{
		whMutexFini(&mutex_);
	}
	WhMutexParam mutex_;
};

class WhCond
{
public:
	WhCond()
	{
		whCondInit(&cond_);
	}
	~WhCond()
	{
		whCondFini(&cond_);
	}
	void wait(WhMutex *m)
	{
		whCondWait(&cond_, m);
	}
	void waitTimed(WhMutex *m, unsigned int secs)
	{
		whCondWaitTimed(&cond_, m, secs);
	}
	void waitTimed(WhMutex *m, WhTimeVal &tv)
	{
		long msec = tv.t_.tv_sec * 1000 + tv.t_.tv_usec / 1000;
		whCondWaitTimedMsec(&cond_, m, msec);
	}
	void signal()
	{
		whCondSignal(&cond_);
	}
	WhCondParam cond_;
};

class WhMutexGuard
{
	WhMutex *m_;
public:
	WhMutexGuard(WhMutex *m)
	{
		m_ = m;
		whMutexEnter(m_);
	}
	~WhMutexGuard()
	{
		whMutexLeave(m_);
	}
};

template<class T>
class JyMutexList
{
protected:
	list<T> list_;
	WhMutex mutex_;
public:
	void pushBack(T &t)
	{
		WhMutexGuard g(&mutex_);
		list_.push_back(t);
	}
	int popFront(T &t)
	{
		WhMutexGuard g(&mutex_);
		if (list_.size() < 1)
			return 0;
		t = list_.front();
		list_.pop_front();
		return 1;
	}
};

template<class T>
class JyMutexWaitList : public JyMutexList<T>
{
protected:
	WhCond cond_;
public:
	void pushBack(T &t)
	{
		WhMutexGuard g(&this->mutex_);
		this->list_.push_back(t);
		if (this->list_.size() == 1)
			cond_.signal();
	}
	int popFrontWait(T &t)
	{
		WhMutexGuard g(&this->mutex_);
		if (this->list_.size() < 1)
		{
			this->cond_.wait(&this->mutex_);
			return 0;
		}
		t = this->list_.front();
		this->list_.pop_front();
		return 1;
	}
	void signal()
	{
		cond_.signal();
	}
};