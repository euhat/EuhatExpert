#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManClient.h"
#include "FmcNewFile.h"
#include <common/JyDataCrypto.h>
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include "../common/FmScheduler.h"
#include <EuhatPostDef.h>

void FmcNewFile::req(FmTaskNew *task)
{
	JyDataWriteStream ds;
	parent_->writeHeader(ds, FmCmdFileNew);
	ds.put(task->id_);
	ds.putStr(task->path_.get());
	ds.put(task->isFolder_);

	parent_->send(parent_->sock_, ds);
}

void FmcNewFile::ack(unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	char *path = ds->getStr();
	int isFolder = ds->get<int>();
	int result = ds->get<int>();

	shared_ptr<FmTaskNew> task;
	if ((task = dynamic_pointer_cast<FmTaskNew>(parent_->scheduler_->get(id))).get() == NULL)
	{
		DBG(("can't find cmd record about [%s] in del.\n", path));
		parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
		return;
	}

	if (result)
	{
		parent_->scheduler_->remove(id);
	}
	else
	{
		task->result_ = FmResultFailed;
	}
	parent_->scheduler_->notifyUi();

	parent_->postMsg(JY_MSG_TYPE_USER_LOOP);
}