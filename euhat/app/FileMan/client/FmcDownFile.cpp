#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManClient.h"
#include "FmcDownFile.h"
#include <common/JyDataCrypto.h>
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include "../common/FmScheduler.h"
#include <EuhatPostDef.h>

void FmcDownFile::req(FmTaskDownload *task)
{
	if (task->offset_ < 0)
	{
		whPrepareFolder(task->localPath_.get());
		opUnlink(task->localPath_.get());

		int64_t totalSpaceInMB = 0;
		int64_t freeSpaceInMB = 0;
		whGetDiskUsage(task->localPath_.get(), totalSpaceInMB, freeSpaceInMB);
		if (freeSpaceInMB - task->totalFileSize_ / (1024 * 1024) < 100)
		{
			task->result_ = FmResultNoSpace;
			DBG(("lack free space to download [%s], need space %lld.\n", task->peerPath_.get(), task->totalFileSize_));
			parent_->scheduler_->notifyUi();
			return;
		}
		task->offset_ = 0;
	}
	JyDataWriteStream ds;
	parent_->writeHeader(ds, FmCmdFileDown);
	ds.put(task->id_);
	ds.putStr(task->peerPath_.get());
	ds.put(task->offset_);
	ds.put((int)FILE_MAN_RECV_FILE_SEGMENT_MAX_LEN);

	parent_->send(parent_->sock_, ds);
}

void FmcDownFile::ack(unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	char *peerPath = ds->getStr();

	shared_ptr<FmTaskDownload> task;
	if ((task = dynamic_pointer_cast<FmTaskDownload>(parent_->scheduler_->get(id))).get() == NULL)
	{
		DBG(("can't find cmd record about [%s].\n", peerPath));
		parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
		return;
	}

	int writeSize = 0;
	int result = ds->get<int>();
	if (result == FmResultOk || result == FmResultFileReachEnd)
	{
		writeSize = fmWriteFile(ds, task->localPath_.get());
		if (writeSize < 0)
			result = FmResultFailed;
		else
			task->offset_ += writeSize;
	}

	if (result == FmResultFileReachEnd)
	{
		whSetFileLastWriteTime(task->localPath_.get(), task->lastWriteTime_);
		parent_->scheduler_->remove(id);
		DBG(("file [%s] download ok.\n", utf8ToGb(task->peerPath_.get()).c_str()));
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
