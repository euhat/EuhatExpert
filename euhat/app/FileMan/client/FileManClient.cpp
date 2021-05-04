 #include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <time.h>
#include "FileManClient.h"
#include <common/JyDataCrypto.h>
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include "../common/FmTask.h"
#include "../common/FmScheduler.h"
#include <EuhatPostDef.h>

FileManClient::FileManClient(FmScheduler *scheduler)
	: JyTcpClient("", -1, FILE_MAN_MEM_POOL_UNIT_SIZE, 1)
{
	downFileOp_.parent_ = this;
	upFileOp_.parent_ = this;
	peerFileListOp_.parent_ = this;
	delFileOp_.parent_ = this;
	newFileOp_.parent_ = this;
	maxPacketNeedTime_ = 0;

	sock_ = WH_INVALID_SOCKET;

	scheduler_ = scheduler;

	canSend_ = 0;
	isUnix_ = 0;

	createTimer(1, 5000);
}

FileManClient::~FileManClient()
{
	stop();
}

void FileManClient::setDbOp(DbOpSqlite *db)
{
	JyTcpClient::setDbOp(db);
}

void FileManClient::onConnectEnd(JyMsg &msg)
{
	WhSockHandle sock = (WhSockHandle)msg.int_;
	
	shared_ptr<FmTaskConnect> task;
	if ((task = dynamic_pointer_cast<FmTaskConnect>(scheduler_->getByType(FmTask::TypeConnect))).get() != NULL)
	{
		if (WH_INVALID_SOCKET == sock)
			task->retryTimes_++;
		else
			task->retryTimes_ = -1;
		scheduler_->notifyUi();
	}

	JyTcpClient::onConnectEnd(msg);
}

void FileManClient::onReadExchangeSymSecurity(unique_ptr<JyDataReadBlock> &ds)
{
	if (!canSend(sock_))
		return;
	if (NULL == ds.get())
		return;

	int isVisitCodeOk = ds->get<int>();
	shared_ptr<FmTaskConnect> task;
	if ((task = dynamic_pointer_cast<FmTaskConnect>(scheduler_->getByType(FmTask::TypeConnect))).get() != NULL)
	{
		if (isVisitCodeOk)
			scheduler_->remove(task->id_);
		else
		{
			task->result_ = FmResultVisitCodeMismatch;
			postMsg(JY_MSG_TYPE_SOCK_DISCONNECT, NULL, sock_);
		}
		scheduler_->notifyUi();
	}

	if (isVisitCodeOk)
	{
		int len = ds->get<int>();
		JyDataReadStream dsRead;
		dsRead.buf_.reset(ds->buf_.data_.get() + ds->getIdx(), len);
		scheduler_->user_->fmscOnGetSysInfo(this, dsRead);
		dsRead.buf_.data_.release();
	}

	JyTcpClient::onReadExchangeSymSecurity(ds);
}

void FileManClient::onUserStart(JyMsg &msg)
{
	if (!canSend(sock_))
		return;

	canSend_ = 1;

	postMsg(JY_MSG_TYPE_USER_LOOP);
}

void FileManClient::onUserLoop(JyMsg &msg)
{
	if (!canSend(sock_))
		return;

	if (!canSend_)
		return;

	shared_ptr<FmTask> task = scheduler_->getFirst();

	if (NULL == task.get())
		return;

	lastRecvTime_ = time(NULL);

	switch (task->type_)
	{
	case FmTask::TypeDownload: 
	{
		FmTaskDownload *t = (FmTaskDownload *)task.get();
		if (t->totalFileSize_ > 1024)
			bigDownloadTask_ = dynamic_pointer_cast<FmTaskDownload>(task);
		downFileOp_.req(t);
		break;
	}
	case FmTask::TypeUpload:
	{
		FmTaskUpload *t = (FmTaskUpload *)task.get();
		if (t->totalFileSize_ > 1024)
			bigUploadTask_ = dynamic_pointer_cast<FmTaskUpload>(task);
		upFileOp_.req(t);
		break;
	}
	case FmTask::TypeGetPeerSubList: peerFileListOp_.req((FmTaskGetSubList *)task.get()); break;
	case FmTask::TypeDelPeer: delFileOp_.req((FmTaskDel *)task.get()); break;
	case FmTask::TypeNewPeer: newFileOp_.req((FmTaskNew *)task.get()); break;
	}

	canSend_ = 0;
}

void FileManClient::onRead(short packetType, unique_ptr<JyDataReadBlock> &ds)
{
	time_t now = time(NULL);
	if (now - lastRecvTime_ > maxPacketNeedTime_)
		maxPacketNeedTime_ = now - lastRecvTime_;
	lastRecvTime_ = now;

	switch (packetType)
	{
	case FmCmdFileDown:
	{
		downFileOp_.ack(ds);
		if (NULL != bigDownloadTask_.get())
		{
			bigDownloadTask_->eatGuess_ = 0;
			bigDownloadTask_.reset();
		}
		break;
	}
	case FmCmdFileUp:
	{
		upFileOp_.ack(ds);
		if (NULL != bigUploadTask_.get())
		{
			bigUploadTask_->eatGuess_ = 0;
			bigUploadTask_.reset();
		}
		break;
	}
	case FmCmdGetPeerFileList: peerFileListOp_.ack(ds); break;
	case FmCmdFileDel: delFileOp_.ack(ds); break;
	case FmCmdFileNew: newFileOp_.ack(ds); break;
	default: break;
	}

	canSend_ = 1;
}

void FileManClient::onTotalSend(JyMsg &msg)
{
	int totalSent = (int)msg.int_;
	if (NULL != bigUploadTask_.get())
	{
		bigUploadTask_->eatGuess_ += totalSent;
	}
	scheduler_->notifyUi();
}

void FileManClient::onTotalRecv(JyMsg &msg)
{
	int totalRecv = (int)msg.int_;
	if (NULL != bigDownloadTask_.get())
	{
		bigDownloadTask_->eatGuess_ += totalRecv;
	}
	scheduler_->notifyUi();
}

void FileManClient::onTimer(JyMsg &msg)
{
	shared_ptr<FmTask> task = scheduler_->getFirst();

	time_t now = time(NULL);
	if (NULL == task.get())
	{
		return;
	}

	if (maxPacketNeedTime_ == 0)
	{

	}
	else
	{
		time_t waitTime = now - lastRecvTime_ - maxPacketNeedTime_;
		if (waitTime > 15)
		{
			scheduler_->user_->fmscNeedReconnect();
		}
	}
}