#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManClient.h"
#include "FmcPeerFileList.h"
#include <common/JyDataEncrypt.h>
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include "../common/FmScheduler.h"
#include "json/json.h"
#include <EuhatPostDef.h>

void FmcPeerFileList::req(FmTaskGetSubList *task)
{
	JyDataWriteStream ds;
	parent_->writeHeader(ds, FmCmdGetPeerFileList);
	ds.put(task->id_);
	ds.putStr(task->path_.get());

	parent_->send(parent_->sock_, ds);
}

void FmcPeerFileList::ack(unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	char *peerPath = ds->getStr();

	shared_ptr<FmTaskGetSubList> task;
	if ((task = dynamic_pointer_cast<FmTaskGetSubList>(parent_->scheduler_->get(id))).get() == NULL)
	{
		DBG(("can't find cmd record about [%s].\n", peerPath));
		parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
		return;
	}

	int isOk = ds->get<int>();
	int len = ds->get<int>();
	JyDataReadStream dsRead;
	dsRead.buf_.reset(ds->buf_.data_.get() + ds->getIdx(), len);

	if (isOk)
	{
		parent_->scheduler_->remove(id);

		dispatchSubList(parent_->scheduler_, parent_, *task.get(), dsRead);
	}
	else
	{
		task->result_ = FmResultFailed;
	}

	dsRead.buf_.data_.release();

	parent_->scheduler_->notifyUi();

	parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
}
