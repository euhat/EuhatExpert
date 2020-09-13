#pragma once

#include <common/JyTcpServer.h>

enum SimpleEchoServerCmdType
{
	SES_CMD_ECHO = JY_MSG_TYPE_USER,
};

class SimpleEchoServer : public JyTcpServer
{
protected:
	void onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds);

public:
	SimpleEchoServer(int serverPort);
	virtual ~SimpleEchoServer();
};