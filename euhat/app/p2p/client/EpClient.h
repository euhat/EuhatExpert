#pragma once

#include <common/JyMsgLoop.h>
#include <common/JyUdpSelector.h>

#include "../share/EpPeer.h"

enum EpClientCmdType
{
	EPCLIENT_CMD_ALLOC = JY_MSG_TYPE_USER,
	EPCLIENT_CMD_CONNECT,
	EPCLIENT_CMD_PEER_CALL
};

class EpClient : public JyMsgLoop
{
	string serverIp_;
	int serverPort_;
	string localIp_;
	int localPort_;
	string id_;
	string peerId_;

	WhSockHandle sock_;
	WhSockAddrIn sendAddr_;
	WhSockAddrIn recvAddr_;
	EpClientEntity peer_;

	JyUdpSelector selector_;

protected:
	int onWork(JyMsg &msg);
	int onInit(JyMsg &msg);
	int onFini(JyMsg &msg);
	int onAlloc(JyMsg &msg);
	int onConnect(JyMsg &msg);
	int onPeerCall(JyMsg &msg);
	int onPeerCallTcpSrv(JyMsg &msg);
	int onPeerCallTcpCli(JyMsg &msg);
	int onTimer(JyMsg &msg);
	int onSockRead(JyMsg &msg);

public:
	EpClient(const char *serverIp, int serverPort, const char *id, const char *peerId);
	~EpClient();

};