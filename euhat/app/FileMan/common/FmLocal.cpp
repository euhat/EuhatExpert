#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FmLocal.h"
#include "FmTask.h"
#include "FmScheduler.h"
#include "FileManFileOp.h"
#include <os/EuhatFileHandles.h>
#include <EuhatPostDef.h>

FmLocal::FmLocal(FmScheduler *scheduler)
{
	scheduler_ = scheduler;
}

FmLocal::~FmLocal()
{

}

void FmLocal::onUserLoop(JyMsg &msg)
{
	shared_ptr<FmTask> task = scheduler_->getFirst(0);
	if (NULL == task.get())
		return;

	switch (task->type_)
	{
	case FmTask::TypeGetSubList: onGetSubList((FmTaskGetSubList *)task.get()); break;
	case FmTask::TypeDel: onDelFile((FmTaskDel *)task.get()); break;
	case FmTask::TypeNew: onNewFile((FmTaskNew *)task.get()); break;
	case FmTask::TypeCopy: break;
	}
}

void FmLocal::onDelFile(FmTaskDel *task)
{
	int result;
	if (task->isFolder_)
		result = opRmDir(task->path_.get());
	else
		result = opUnlink(task->path_.get());
	if (result)
	{
		scheduler_->remove(task->id_);
	}
	else
	{
		task->result_ = FmResultFailed;
	}
	scheduler_->notifyUi();
	postMsg(JY_MSG_TYPE_USER_LOOP);
}

void FmLocal::onNewFile(FmTaskNew *task)
{
	int result;
	string path = task->path_.get();
	if (task->isFolder_)
	{
		whPrepareFolder(path.c_str());
		result = opMkDir(path.c_str());
	}
	else
	{
		WhFileGuard g(EuhatFileHandles::getInstance()->fopen(path.c_str(), "wb+"));
		result = (g.fp_ != NULL);
	}
	if (task->needInformUi_)
	{
		if (result)
		{
			scheduler_->remove(task->id_);
		}
		else
		{
			task->result_ = FmResultFailed;
		}
		scheduler_->notifyUi();
	}
	else
	{
		scheduler_->remove(task->id_);
	}
	postMsg(JY_MSG_TYPE_USER_LOOP);
}

void FmLocal::onGetSubList(FmTaskGetSubList *task)
{
	JyDataWriteStream ds;
	int isOk = whGetSubFiles(task->path_.get(), ds);
	if (!isOk)
	{
		task->result_ = FmResultFailed;
		scheduler_->notifyUi();
		return;
	}

	JyDataReadStream dsRead;
	dsRead.buf_.reset(ds.buf_.data_.get(), ds.getIdx());

	dispatchSubList(scheduler_, this, *task, dsRead);

	dsRead.buf_.data_.release();

	scheduler_->remove(task->id_);

	scheduler_->notifyUi();

	postMsg(JY_MSG_TYPE_USER_LOOP);
}

void FmLocal::onInit(JyMsg &msg)
{
	createTimer(1, 1000);
}

void FmLocal::onTimer(JyMsg &msg)
{
	scheduler_->onTimer();
}

int FmLocal::onWork(JyMsg &msg)
{
	switch (msg.cmd_)
	{
	case JY_MSG_TYPE_INIT: onInit(msg); return 1;
	case JY_MSG_TYPE_TIMER: onTimer(msg); return 1;
	case JY_MSG_TYPE_USER_LOOP: onUserLoop(msg); return 1;
	}
	return 1;
}