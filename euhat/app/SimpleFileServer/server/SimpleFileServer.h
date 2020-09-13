#pragma once

#include <common/JyTcpServer.h>
#include <common/JyDataStream.h>

enum SimpleFileServerCmdType
{
	SFS_CMD_LISTEN = JY_MSG_TYPE_USER,
};

class SimpleFileServer : public JyTcpServer
{
protected:
	int onWork(JyMsg &msg);

	void onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds);
	void onRecvFileReq(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);

public:
	SimpleFileServer(int serverPort);
	virtual ~SimpleFileServer();

};