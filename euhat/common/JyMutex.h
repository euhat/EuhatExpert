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
public:
	WhMutexGuard(WhMutex *cs)
	{
		cs_ = cs;
		if (NULL == cs_)
			return;
		whMutexEnter(cs_);
	}
	~WhMutexGuard()
	{
		if (NULL == cs_)
			return;
		whMutexLeave(cs_);
	}
	WhMutex *cs_;
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

template<class T, class K>
class JyHistoryList
{
public:
	JyHistoryList(int useLock = 1)
	{
		if (useLock)
			cs_ = new WhMutex();
		else
			cs_ = NULL;
	}
	~JyHistoryList()
	{
		if (NULL != cs_)
			delete cs_;
	}
	struct JyHistorySearch
	{
		JyHistorySearch(K key)
		{
			key_ = key;
		}
		bool operator()(T &entry)
		{
			if (entry.key_ == key_)
				return true;
			return false;
		}
		K key_;
	};
	int remove(K key)
	{
		WhMutexGuard g(cs_);
		typename list<T>::iterator it = remove_if(list_.begin(), list_.end(), JyHistorySearch(key));
		if (it == list_.end())
			return 0;
		list_.erase(it, list_.end());
		return 1;
	}
	T *find(K key)
	{
		WhMutexGuard g(cs_);
		typename list<T>::iterator it = find_if(list_.begin(), list_.end(), JyHistorySearch(key));
		if (it != list_.end())
			return &*it;
		return NULL;
	}
	T *add(K key)
	{
		WhMutexGuard g(cs_);
		T *t = find(key);
		if (NULL == t)
			t = newEntry(key);
		return t;
	}
	T *newEntry(K key)
	{
		WhMutexGuard g(cs_);
		list_.push_back(T());
		T *entry = &list_.back();
		entry->key_ = key;
		return entry;
	}
	list<T> list_;
	WhMutex *cs_;
};