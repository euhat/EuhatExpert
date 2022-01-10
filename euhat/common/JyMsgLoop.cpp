#include <EuhatPreDef.h>
#include "JyMsgLoop.h"
#include <common/JyMutex.h>
#include <time.h>
#include <EuhatPostDef.h>

struct JyMsgTimerSlot
{
	WhTimeVal at_;
	int id_;
	int isRepeat_;
	WhTimeVal interval_;
};

class JyMsgTimer : public JyWorkThread
{
protected:
	int workProc();
	JyMsgTimerSlot *insertSlot(WhTimeVal &atTime);

public:
	~JyMsgTimer();
	int addSlot(int id, WhTimeVal &interval, int isRepeat);
	int removeSlot(int id);

	JyMsgLoop *parent_;
	list<JyMsgTimerSlot> schedule_;
	WhMutex mutex_;
	WhCond cond_;
};

int JyMsgTimer::workProc()
{
	while (workThreadIsRunning_)
	{
		JyMsgTimerSlot slot;
		slot.id_ = -1;
		{
			WhMutexGuard guard(&mutex_);
			if (schedule_.size() == 0)
				cond_.wait(&mutex_);
			else
			{
				WhTimeVal now;
				now.now();
				if (now.biggerThan(schedule_.front().at_))
				{
					slot = schedule_.front();
					schedule_.pop_front();

					if (slot.isRepeat_)
					{
						JyMsgTimerSlot *slotNew = insertSlot(now.add(slot.interval_));
						slotNew->isRepeat_ = slot.isRepeat_;
						slotNew->interval_ = slot.interval_;
						slotNew->id_ = slot.id_;
					}
				}
				else
					cond_.waitTimed(&mutex_, schedule_.front().at_.sub(now));
			}
		}
		if (slot.id_ == -1)
			continue;

		parent_->postMsg(JY_MSG_TYPE_TIMER, NULL, slot.id_);
	}

	return 1;
}

JyMsgTimer::~JyMsgTimer()
{
	workThreadIsRunning_ = 0;
	cond_.signal();
	stop();
}

JyMsgTimerSlot *JyMsgTimer::insertSlot(WhTimeVal &atTime)
{
	auto it = find_if(schedule_.begin(), schedule_.end(), [&atTime](JyMsgTimerSlot &slot) { return atTime.biggerThan(slot.at_); });
	if (it != schedule_.end())
		it++;

	JyMsgTimerSlot *slot;
	if (it == schedule_.end())
	{
		schedule_.push_back(JyMsgTimerSlot());
		slot = &schedule_.back();
	}
	else
	{
		it = schedule_.insert(it, JyMsgTimerSlot());
		slot = &*it;
	}
	slot->at_ = atTime;
	return slot;
}

int JyMsgTimer::removeSlot(int id)
{
	WhMutexGuard guard(&mutex_);

	schedule_.erase(remove_if(schedule_.begin(), schedule_.end(), [id](JyMsgTimerSlot &slot) { return id == slot.id_; }), schedule_.end());
	return 1;
}

int JyMsgTimer::addSlot(int id, WhTimeVal &interval, int isRepeat)
{
	if (!isRepeat && id >= JY_MSG_TYPE_TIMER)
	{
		DBG(("delay work id %d is illegal.\n", id));
		return 0;
	}

	WhMutexGuard guard(&mutex_);

	JyMsgTimerSlot *slot = insertSlot(WhTimeVal().now().add(interval));
	slot->isRepeat_ = isRepeat;
	slot->interval_ = interval;
	slot->id_ = id;

	if (schedule_.size() == 1)
		cond_.signal();

	return 1;
}

JyMsgLoop::JyMsgLoop()
	: timer_(new JyMsgTimer)
{
	timer_->parent_ = this;
}

JyMsgLoop::~JyMsgLoop()
{
	DBG(("~JyMsgLoop call in.\n"));
	stop();
}

int JyMsgLoop::start()
{
	JyWorkQueue::start();

	postMsg(JY_MSG_TYPE_INIT);

	timer_->start();
	return 1;
}

int JyMsgLoop::stop()
{
	if (workThreadHandle_ == WH_INVALID_THREAD)
		return 0;

	timer_.reset();

	postMsg(JY_MSG_TYPE_FINI);

	return JyWorkQueue::stop();
}

int JyMsgLoop::quit()
{
	postMsg(JY_MSG_TYPE_FINI);

	workThreadIsRunning_ = 0;

	return 1;
}

void JyMsgLoop::postMsg(int cmd, void *p, vint i)
{
	JyMsg msg;
	msg.cmd_ = cmd;
	msg.p_ = p;
	msg.int_ = i;
	pushBack(msg);
}

int JyMsgLoop::onWork(JyMsg &msg)
{
	return 1;
}

int JyMsgLoop::createTimer(int id, long msecs)
{
	WhTimeVal tv;
	tv.t_.tv_sec = msecs / 1000;
	tv.t_.tv_usec = (msecs % 1000) * 1000;
	return timer_->addSlot(id, tv, 1);
}

int JyMsgLoop::killTimer(int id)
{
	return cancelWork(id);
}

int JyMsgLoop::delayWork(int type, long msecs)
{
	WhTimeVal tv;
	tv.t_.tv_sec = msecs / 1000;
	tv.t_.tv_usec = (msecs % 1000) * 1000;
	return timer_->addSlot(type, tv, 0);
}

int JyMsgLoop::cancelWork(int type)
{
	timer_->removeSlot(type);

	WhMutexGuard guard(&mutex_);
	list_.erase(remove_if(list_.begin(), list_.end(), [type](JyMsg &msg){ return msg.cmd_ == JY_MSG_TYPE_TIMER && msg.int_ == type; }), list_.end());

	return 1;
}