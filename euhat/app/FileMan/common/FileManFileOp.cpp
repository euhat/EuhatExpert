#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManFileOp.h"
#include "FileManType.h"
#include <common/JyBigNum.h>
#include <dbop/DbOpSqliteTable.h>
#include <dbop/DbOpIni.h>
#include <common/JyTcpSelector.h>
#include "FmScheduler.h"
#include "FmTask.h"
#include "../client/FileManClient.h"
#include <os/EuhatPath.h>
#include <time.h>
#include <EuhatPostDef.h>

void fmReadFile(JyDataWriteStream &ds, const char *path, int64_t pos, int len)
{
	int result = FmResultOk;

	WhFileGuard g(whFopen(path, "rb"));
	if (NULL == g.fp_)
	{
		ds.put((int)FmResultFileNotExist);
		return;
	}

	int64_t endPos = whGetFileSize(path);
	if (endPos < pos)
	{
		ds.put((int)FmResultFileSeekExceed);
		return;
	}

	whFseek64(g.fp_, pos, SEEK_SET);
/*	int64_t truePos = whFtell64(g.fp_);

	if (truePos != pos)
	{
		ds.put((int)FmResultFileSeekError);
		return;
	}
*/

	int64_t trueLeft = endPos - pos;
	if (trueLeft > (int64_t)len)
		trueLeft = len;
	else
		result = FmResultFileReachEnd;

	int toReadLen = (int)trueLeft;

	ds.put(result);
	ds.put(pos);
	char *ptr = ds.allocBuf(toReadLen);
	fread(ptr, 1, toReadLen, g.fp_);

	return;
}

int fmWriteFile(unique_ptr<JyDataReadBlock> &ds, const char *path)
{
	WhFileGuard g(whFopen(path, "ab+"));
	if (NULL == g.fp_)
		return -1;

	int64_t pos = ds->get<int64_t>();
	int len;
	char *buf = ds->getBuf(len);

	int64_t fileSize = whGetFileSize(path);
	if (pos != fileSize)
	{
		if (pos + len == fileSize)
			return len;
		return -1;
	}

	DBG(("fm write file [%s], len %d.\n", path, len));
	fwrite(buf, 1, len, g.fp_);

	return len;
}

void fmGenAndWriteCert(DbOpIni &ini, const char *keyName, int bits)
{
	JyBigNumCtx ctx;

	JyBigNum p(ctx);
	p.genPrime(bits);

	JyBigNum q(ctx);
	q.genPrime(bits);
	
	JyBigNum n(ctx);
	n.mul(p, q);

	p.sub(1);
	q.sub(1);

	JyBigNum phi(ctx);
	phi.mul(p, q);

	JyBigNum e(ctx);
	JyBigNum::getE(e, bits);

	JyBigNum d(ctx);
	d.modInverse(e, phi);

	JyBuf buf;
	n.getBuf(buf);
	string key = string(keyName) + "_n";
	ini.writeBuf(JY_TCP_INI_CERT_SECTION, key.c_str(), buf);

	e.getBuf(buf);
	key = string(keyName) + "_e";
	ini.writeBuf(JY_TCP_INI_CERT_SECTION, key.c_str(), buf);

	d.getBuf(buf);
	key = string(keyName) + "_d";
	ini.writeBuf(JY_TCP_INI_CERT_SECTION, key.c_str(), buf);

	key = string(keyName) + "_lastGenDate";
	time_t t = time(NULL);
	ini.write(JY_TCP_INI_CERT_SECTION, key.c_str(), time2Str(t).c_str());
}

static char *combinePath(FmScheduler *scheduler, JyMsgLoop *loop, char *curDir, char *name)
{
	int isUnix = 0;
	if (loop == scheduler->remote_)
		isUnix = ((FileManClient *)loop)->isUnix_;
	EuhatPath path;
	path.isUnix_ = isUnix;
	path.inStr(curDir);
	path.goSub(name);
	return opStrDup(path.toStr().c_str());
}

void dispatchSubList(FmScheduler *scheduler, JyMsgLoop *loop, FmTaskGetSubList &task, JyDataReadStream &ds)
{
	if (task.action_ == FmTaskGetSubList::ActionInformUi)
	{
		scheduler->user_->fmscOnGetSubList(loop, ds);
		return;
	}
	char *curDir = ds.getStr();
	while (1)
	{
		char type = ds.get<char>();
		if (type == 0)
		{
			char *name = ds.getStr();

			if (task.action_ == FmTaskGetSubList::ActionDel)
			{
				shared_ptr<FmTaskGetSubList> t(new FmTaskGetSubList(loop == scheduler->remote_ ? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
				t->path_.reset(combinePath(scheduler, loop, curDir, name));
				t->action_ = FmTaskGetSubList::ActionDel;
				scheduler->add(t);
			}
			else if (task.action_ == FmTaskGetSubList::ActionDownUp)
			{
				shared_ptr<FmTaskNew> tNew(new FmTaskNew(loop == scheduler->remote_ ? FmTask::TypeNew : FmTask::TypeNewPeer));
				tNew->path_.reset(combinePath(scheduler, loop == scheduler->remote_ ? scheduler->local_ : scheduler->remote_, task.peerPath_.get(), name));
				tNew->isFolder_ = 1;
				tNew->needInformUi_ = 0;
				scheduler->add(tNew);

				shared_ptr<FmTaskGetSubList> t(new FmTaskGetSubList(loop == scheduler->remote_ ? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
				t->path_.reset(combinePath(scheduler, loop, curDir, name));
				t->action_ = FmTaskGetSubList::ActionDownUp;
				t->peerPath_.reset(combinePath(scheduler, loop == scheduler->remote_ ? scheduler->local_ : scheduler->remote_, task.peerPath_.get(), name));
				t->isMove_ = task.isMove_;
				t->action_ = FmTaskGetSubList::ActionDownUp;
				scheduler->add(t);
			}
			else if (task.action_ == FmTaskGetSubList::ActionCopy)
			{
				shared_ptr<FmTaskGetSubList> t(new FmTaskGetSubList(loop == scheduler->remote_ ? FmTask::TypeGetPeerSubList : FmTask::TypeGetSubList));
				t->path_.reset(combinePath(scheduler, loop, curDir, name));
				t->action_ = FmTaskGetSubList::ActionCopy;
				t->peerPath_.reset(combinePath(scheduler, loop == scheduler->remote_ ? scheduler->local_ : scheduler->remote_, task.peerPath_.get(), name));
				t->isMove_ = task.isMove_;
				t->action_ = FmTaskGetSubList::ActionCopy;
				scheduler->add(t);
			}
		}
		else if (type == 1)
		{
			char *name = ds.getStr();
			int64_t size = ds.get<int64_t>();
			int64_t lastWriteTime = ds.get<int64_t>();

			if (task.action_ == FmTaskGetSubList::ActionDel)
			{
				shared_ptr<FmTaskDel> task(new FmTaskDel(loop == scheduler->remote_ ? FmTask::TypeDelPeer : FmTask::TypeDel));
				task->path_.reset(combinePath(scheduler, loop, curDir, name));
				task->isFolder_ = 0;
				task->priority_ = 100;
				scheduler->add(task);
			}
			else if (task.action_ == FmTaskGetSubList::ActionDownUp)
			{
				if (loop == scheduler->remote_)
				{
					shared_ptr<FmTaskDownload> t(new FmTaskDownload());
					t->peerPath_.reset(combinePath(scheduler, loop, curDir, name));
					t->localPath_.reset(combinePath(scheduler, loop == scheduler->remote_ ? scheduler->local_ : scheduler->remote_, task.peerPath_.get(), name));
					t->offset_ = -1;
					t->totalFileSize_ = size;
					t->lastWriteTime_ = lastWriteTime;
					t->isMove_ = task.isMove_;
					scheduler->add(t);
				}
				else
				{
					shared_ptr<FmTaskUpload> t(new FmTaskUpload());
					t->localPath_.reset(combinePath(scheduler, loop, curDir, name));
					t->peerPath_.reset(combinePath(scheduler, loop == scheduler->remote_ ? scheduler->local_ : scheduler->remote_, task.peerPath_.get(), name));
					t->offset_ = -1;
					t->totalFileSize_ = size;
					t->lastWriteTime_ = lastWriteTime;
					t->isMove_ = task.isMove_;
					scheduler->add(t);
				}
			}
			else if (task.action_ == FmTaskGetSubList::ActionCopy)
			{
				shared_ptr<FmTaskCopy> t(new FmTaskCopy(loop == scheduler->remote_ ? FmTask::TypeCopyPeer : FmTask::TypeCopy));
				t->fromPath_.reset(combinePath(scheduler, loop, curDir, name));
				t->toPath_.reset(combinePath(scheduler, loop, task.peerPath_.get(), name));
				t->offset_ = -1;
				t->totalFileSize_ = size;
				scheduler->add(t);
			}
		}
		else
			break;
	}

	if (task.action_ == FmTaskGetSubList::ActionDel ||
		(task.action_ == FmTaskGetSubList::ActionDownUp && task.isMove_))
	{
		shared_ptr<FmTaskDel> task(new FmTaskDel(loop == scheduler->remote_ ? FmTask::TypeDelPeer : FmTask::TypeDel));
		EuhatPath euPath;
		euPath.inStr(curDir);
		task->priority_ = 1000 - euPath.path_.size();
		task->path_.reset(opStrDup(curDir));
		task->isFolder_ = 1;
		scheduler->add(task);
	}

	if (task.action_ == FmTaskGetSubList::ActionDownUp)
	{
		shared_ptr<FmTaskNew> tNew(new FmTaskNew(loop == scheduler->remote_ ? FmTask::TypeNew : FmTask::TypeNewPeer));
		tNew->path_.reset(opStrDup(task.peerPath_.get()));
		tNew->isFolder_ = 1;
		tNew->needInformUi_ = 0;
		scheduler->add(tNew);
	}

	scheduler->notifyEngine();
	scheduler->notifyUi();
}
