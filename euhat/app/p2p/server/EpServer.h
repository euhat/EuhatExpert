#pragma once

#include <common/JyMsgLoop.h>
#include <common/JyUdpSelector.h>
#include "../share/EpPeer.h"

enum EpServerCmdType
{
	EPSERVER_CMD_ALLOC = JY_MSG_TYPE_USER
};

class EpServer : public JyMsgLoop
{
	int port_;

	WhSockHandle sock_;
	WhSockHandle sockRelay_;
	WhSockAddrIn sendAddr_;

	JyUdpSelector selector_;
	list<EpClientEntity> clients_;

protected:
	int onWork(JyMsg &msg);
	int onInit(JyMsg &msg);
	int onFini(JyMsg &msg);
	int onSockRead(JyMsg &msg);
	int onTimer(JyMsg &msg);

public:
	EpServer(int port);
	~EpServer();

};