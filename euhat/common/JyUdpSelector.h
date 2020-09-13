#pragma once

#include "JySockSelector.h"

struct JyUdpRecvBuf
{
	WhSockAddrIn recvAddr_;
	char *buf_;
	int bufLen_;
	int recvLen_;

	JyUdpRecvBuf();
	~JyUdpRecvBuf();
};

struct JyUdpSendBuf
{
	WhSockAddrIn sendAddr_;
	char *buf_;
	int bufLen_;
	int sendLen_;

	JyUdpSendBuf();
	~JyUdpSendBuf();
};

struct JyUdpListener
{
	JyMsgLoop *listener_;
	list<unique_ptr<JyUdpSendBuf> > sendQueue_;

	JyUdpListener(JyMsgLoop *listener);
	~JyUdpListener();
};

class JyUdpSelector : public JySockSelector
{
	map<WhSockHandle, unique_ptr<JyUdpListener> > socks_;

protected:
	int onRead(WhSockHandle sock);
	int onWrite(WhSockHandle sock);
	int send(WhSockHandle sock, unique_ptr<JyUdpSendBuf> &buf);

public:
	~JyUdpSelector();

	WhSockHandle bind(int port);
	int add(WhSockHandle sock, JyMsgLoop *listener);
	int send(WhSockHandle sock, WhSockAddrIn &sockAddr, const char *buf, int bufLen);
	int send(WhSockHandle sock, WhSockAddrIn &sockAddr, const char *buf);
	int del(WhSockHandle sock);
};