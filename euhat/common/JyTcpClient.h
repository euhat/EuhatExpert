#pragma once

#include "JyTcpEndpoint.h"

class JyTcpClient : public JyTcpEndpoint
{

protected:
	virtual void onInit(JyMsg &msg);
	virtual void onFini(JyMsg &msg);
	virtual void onConnectBegin(JyMsg &msg);
	virtual void onConnectEnd(JyMsg &msg);
	virtual void onExchangeAsymSecurity(JyMsg &msg);
	virtual void onExchangeSymSecurity(JyMsg &msg);
	virtual void onUserStart(JyMsg &msg);
	virtual void onUserLoop(JyMsg &msg);
	virtual void onDisconnect(JyMsg &msg);
	virtual void onSockRead(JyMsg &msg);
	virtual void onTotalSend(JyMsg &msg) {}
	virtual void onTotalRecv(JyMsg &msg) {}
	virtual void onTimer(JyMsg &msg) {}

	virtual void onRead(short packetType, unique_ptr<JyDataReadBlock> &ds);
	void onReadExchangeAsymSecurity(unique_ptr<JyDataReadBlock> &ds);
	virtual void onReadExchangeSymSecurity(unique_ptr<JyDataReadBlock> &ds);

	virtual int onWork(JyMsg &msg);

	WhSockHandle sock_;

public:
	JyTcpClient(const char *serverIp, int port, int poolUnitSize, int isSecure = 0);
	virtual ~JyTcpClient();

	string serverIp_;
	int serverPort_;
	string visitCode_;
};