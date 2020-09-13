#pragma once

#include <common/JyTcpClient.h>

enum SimpleEchoClientCmdType
{
	SEC_CMD_ECHO = JY_MSG_TYPE_USER,
};

class SimpleEchoClient : public JyTcpClient
{
protected:
	void onUserStart(JyMsg &msg);
	void onRead(short packetType, unique_ptr<JyDataReadBlock> &ds);

public:
	SimpleEchoClient(const char *serverIp, int serverPort);
	virtual ~SimpleEchoClient();
};