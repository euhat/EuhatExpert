#pragma once

#include "JyTcpEndpoint.h"

class JyTcpServer : public JyTcpEndpoint
{

protected:
	virtual void onInit(JyMsg &msg);
	virtual void onFini(JyMsg &msg);
	virtual void onListen(JyMsg &msg);
	virtual void onSockAccept(JyMsg &msg);
	virtual void onDisconnect(JyMsg &msg);
	virtual void onSockRead(JyMsg &msg);

	virtual void onWriteHostInfo(JyDataWriteStream &dsAck);

	virtual void onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds);
	void onReadExchangeAsymSecurity(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);
	void onReadExchangeSymSecurity(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);

	virtual int onWork(JyMsg &msg);

public:
	JyTcpServer(int port, int poolUnitSize, int isSecure = 0);
	virtual ~JyTcpServer();

	int serverPort_;
	string visitCode_;
};