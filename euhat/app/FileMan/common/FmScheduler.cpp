#include <EuhatPreDef.h>
#include "FmScheduler.h"
#include "FmTask.h"
#include <common/JyMsgLoop.h>
#include <EuhatPostDef.h>

void FmScheduler::UserWrapper::fmscOnGetSubList(JyMsgLoop *loop, JyDataReadStream &ds)
{
	FmscUserParam *param = new FmscUserParam();
	param->cmd_ = 0;

	size_t len = ds.buf_.size_;
	char *mem = (char *)malloc(len);
	memcpy(mem, ds.buf_.data_.get(), len);

	param->i1_ = len;
	param->i2_ = (int64_t)mem;
	param->p_ = loop;

	listMsg_.pushBack(param);
	user_->fmscNotify();
}

void FmScheduler::UserWrapper::fmscOnRefresh()
{
	FmscUserParam *param = new FmscUserParam();
	param->cmd_ = 1;

	listMsg_.pushBack(param);
	user_->fmscNotify();
}

void FmScheduler::UserWrapper::fmscNeedReconnect()
{
	FmscUserParam *param = new FmscUserParam();
	param->cmd_ = 2;

	listMsg_.pushBack(param);
	user_->fmscNotify();
}

void FmScheduler::UserWrapper::fmscOnGetSysInfo(JyMsgLoop *loop, JyDataReadStream &ds)
{
	FmscUserParam *param = new FmscUserParam();
	param->cmd_ = 3;

	size_t len = ds.buf_.size_;
	char *mem = (char *)malloc(len);
	memcpy(mem, ds.buf_.data_.get(), len);

	param->i1_ = len;
	param->i2_ = (int64_t)mem;
	param->p_ = loop;

	listMsg_.pushBack(param);
	user_->fmscNotify();
}

void FmScheduler::UserWrapper::notify()
{
	FmscUserParam *param;
	int got;
	do
	{
		got = listMsg_.popFront(param);
		if (got)
		{
			switch (param->cmd_)
			{
			case 0:
			{
				JyDataReadStream ds;
				int len = (int)param->i1_;
				char *mem = (char *)param->i2_;
				ds.buf_.data_.reset(mem);
				ds.buf_.size_ = len;

				user_->fmscOnGetSubList((JyMsgLoop *)param->p_, ds);
				break;
			}
			case 1: user_->fmscOnRefresh(); break;
			case 2: user_->fmscNeedReconnect(); break;
			case 3:
			{
				JyDataReadStream ds;
				int len = (int)param->i1_;
				char *mem = (char *)param->i2_;
				ds.buf_.data_.reset(mem);
				ds.buf_.size_ = len;

				user_->fmscOnGetSysInfo((JyMsgLoop *)param->p_, ds);
				break;
			}
			}
			delete param;
		}
	} while (got);
}

FmScheduler::UserWrapper::UserWrapper(User *user)
{
	user_ = user;
}

FmScheduler::UserWrapper::~UserWrapper()
{
	FmscUserParam *param;
	int got;
	do
	{
		got = listMsg_.popFront(param);
		if (got)
			delete param;
	} while (got);
}

FmScheduler::FmScheduler(User *user)
	: user_(new UserWrapper(user))
{
	remote_ = local_ = NULL;
	idxBase_ = 0;
	needNotifyUi_ = 0;
}

FmScheduler::~FmScheduler()
{

}

void FmScheduler::addRefreshTask(shared_ptr<FmTaskGetSubList> task)
{
	WhMutexGuard guard(&mutex_);

	task->id_ = idxBase_++;
	task->result_ = FmResultOk;

	refreshSubListTask_ = task;
}

void FmScheduler::add(shared_ptr<FmTask> task)
{
	WhMutexGuard guard(&mutex_);

	task->id_ = idxBase_++;
	task->result_ = FmResultOk;

	if (tasks_.size() == 0)
	{
		tasks_.push_back(task);

	}
	else
	{
		int isInserted = 0;
		for (list<shared_ptr<FmTask>>::reverse_iterator it = tasks_.rbegin(); it != tasks_.rend(); it++)
		{
			FmTask *t = (*it).get();
			if (task->getPriority() >= t->getPriority())
			{
				tasks_.insert(it.base(), task);
				isInserted = 1;
				break;
			}
		}
		if (!isInserted)
		{
			tasks_.push_front(task);
		}
	}
}

void FmScheduler::cleanError()
{
	WhMutexGuard guard(&mutex_);
	for (list<shared_ptr<FmTask>>::iterator it = tasks_.begin(); it != tasks_.end(); it++)
	{
		FmTask *task = (*it).get();
		if (task->result_ != FmResultOk && task->type_ != FmTask::TypeConnect)
		{
			tasks_.erase(it);
			return;
		}
	}
}

void FmScheduler::cleanAll()
{
	WhMutexGuard guard(&mutex_);
	tasks_.clear();
}

void FmScheduler::remove(int id)
{
	WhMutexGuard guard(&mutex_);

	for (list<shared_ptr<FmTask>>::iterator it = tasks_.begin(); it != tasks_.end(); it++)
	{
		FmTask *task = (*it).get();
		if (task->id_ == id)
		{
			tasks_.erase(it);
			return;
		}
	}
}

shared_ptr<FmTask> FmScheduler::getFirst(int isRemoteTask)
{
	WhMutexGuard guard(&mutex_);
	if (tasks_.size() == 0)
		return NULL;

	if (tasks_.front()->result_ != FmResultOk)
		return NULL;

	for (list<shared_ptr<FmTask>>::iterator it = tasks_.begin(); it != tasks_.end(); it++)
	{
		FmTask *t = (*it).get();

		if (t->type_ == FmTask::TypeCopy ||
			t->type_ == FmTask::TypeGetSubList ||
			t->type_ == FmTask::TypeDel ||
			t->type_ == FmTask::TypeNew)
		{
			if (!isRemoteTask)
			{
				return *it;
			}
		}
		else
		{
			if (isRemoteTask)
			{
				return *it;
			}
		}
	}
	return NULL;
}

shared_ptr<FmTask> FmScheduler::get(int id)
{
	WhMutexGuard guard(&mutex_);
	if (tasks_.size() == 0)
		return NULL;

	for (list<shared_ptr<FmTask>>::iterator it = tasks_.begin(); it != tasks_.end(); it++)
	{
		FmTask *t = (*it).get();

		if (t->id_ == id)
			return *it;
	}
	return NULL;
}

shared_ptr<FmTask> FmScheduler::getByType(FmTask::Type type)
{
	WhMutexGuard guard(&mutex_);
	if (tasks_.size() == 0)
		return NULL;

	for (list<shared_ptr<FmTask>>::iterator it = tasks_.begin(); it != tasks_.end(); it++)
	{
		FmTask *t = (*it).get();

		if (t->type_ == type)
			return *it;
	}
	return NULL;
}

void FmScheduler::notifyUi()
{
	needNotifyUi_ = 1;
}

void FmScheduler::onTimer()
{
	if (needNotifyUi_)
	{
		needNotifyUi_ = 0;
		user_->fmscOnRefresh();
	}
}

void FmScheduler::notifyEngine()
{
	if (NULL != remote_)
		remote_->postMsg(JY_MSG_TYPE_USER_LOOP);
	if (NULL != local_)
		local_->postMsg(JY_MSG_TYPE_USER_LOOP);
}