#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManClient.h"
#include "FmcUpFile.h"
#include <common/JyDataEncrypt.h>
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include "../common/FmScheduler.h"
#include <EuhatPostDef.h>

void FmcUpFile::req(FmTaskUpload *task)
{
	JyDataWriteStream ds;
	parent_->writeHeader(ds, FmCmdFileUp);
	ds.put(task->id_);
	ds.putStr(task->peerPath_.get());
	ds.put(task->lastWriteTime_);
	ds.put(task->totalFileSize_);
	ds.put(task->offset_);
	if (task->offset_ < 0)
	{
	}
	else
	{
		int reqLen = FILE_MAN_SEND_FILE_SEGMENT_MAX_LEN;
		fmReadFile(ds, task->localPath_.get(), task->offset_, reqLen);
	}
	parent_->send(parent_->sock_, ds);
}

void FmcUpFile::ack(unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	char *peerPath = ds->getStr();

	shared_ptr<FmTaskUpload> task;
	if ((task = dynamic_pointer_cast<FmTaskUpload>(parent_->scheduler_->get(id))).get() == NULL)
	{
		DBG(("can't find cmd record about [%s] in upload.\n", peerPath));
		parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
		return;
	}

	task->offset_ = ds->get<int64_t>();
	int result = ds->get<int>();
	if (result == FmResultNoSpace)
	{
		DBG(("server has no free space to save the file [%s][%lld].\n", peerPath, task->totalFileSize_));
		task->result_ = FmResultNoSpace;
	}
	else if (result == FmResultFileReachEnd)
	{
		parent_->scheduler_->remove(id);
	}
	else if (result == FmResultOk)
	{
	}
	else
	{
		task->result_ = FmResultFailed;
	}
	parent_->scheduler_->notifyUi();

	parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
}