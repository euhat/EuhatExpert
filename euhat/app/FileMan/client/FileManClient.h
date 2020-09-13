#pragma once

#include <common/JyTcpClient.h>
#include <common/JyDataStream.h>
#include "FileManCmdTable.h"
#include "FmcDownFile.h"
#include "FmcUpFile.h"
#include "FmcPeerFileList.h"
#include "FmcDelFile.h"
#include "FmcNewFile.h"

enum FileManClientMsgType
{
	FMC_MSG_USER1 = JY_MSG_TYPE_USER
};

class FmScheduler;

class FileManClient : public JyTcpClient
{
private:
	FmcDownFile downFileOp_;
	FmcUpFile upFileOp_;
	FmcPeerFileList peerFileListOp_;
	FmcDelFile delFileOp_;
	FmcNewFile newFileOp_;

	friend class FmcDownFile;
	friend class FmcUpFile;
	friend class FmcPeerFileList;
	friend class FmcDelFile;
	friend class FmcNewFile;

protected:
	void onConnectEnd(JyMsg &msg);
	void onReadExchangeSymSecurity(unique_ptr<JyDataReadBlock> &ds);
	void onUserStart(JyMsg &msg);
	void onUserLoop(JyMsg &msg);
	void onRead(short packetType, unique_ptr<JyDataReadBlock> &ds);
	void onTotalSend(JyMsg &msg);
	void onTotalRecv(JyMsg &msg);
	void onTimer(JyMsg &msg);

	int canSend_;

public:
	FileManClient(FmScheduler *scheduler);
	~FileManClient();
	void setDbOp(DbOpSqlite *db);

	FmScheduler *scheduler_;
	int isUnix_;
	shared_ptr<FmTaskDownload> bigDownloadTask_;
	shared_ptr<FmTaskUpload> bigUploadTask_;
	time_t lastRecvTime_;
	time_t maxPacketNeedTime_;
};