#pragma once

#include <common/JyTcpServer.h>
#include <common/JyDataStream.h>
#include "FmsDownFile.h"
#include "FmsUpFile.h"
#include "FmsPeerFileList.h"
#include "FmsDelFile.h"
#include "FmsNewFile.h"

enum FileManServerCmdType
{
	FMS_CMD_LISTEN = JY_MSG_TYPE_USER,
};

class FileManServer : public JyTcpServer
{
	FmsDownFile downFileOp_;
	FmsUpFile upFileOp_;
	FmsPeerFileList peerFileListOp_;
	FmsDelFile delFileOp_;
	FmsNewFile newFileOp_;

	friend class FmsDownFile;
	friend class FmsUpFile;
	friend class FmsPeerFileList;
	friend class FmsDelFile;
	friend class FmsNewFile;

protected:
	void onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds);
	void onWriteHostInfo(JyDataWriteStream &dsAck);

public:
	FileManServer(int serverPort);
	FileManServer();
	~FileManServer();

	string baseDir_;
};