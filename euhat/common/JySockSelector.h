#pragma once

#include "JyWorkThread.h"
#include <common/JyMutex.h>

class JyMsgLoop;

class JySockSelector : public JyWorkThread
{
	int workProc();

protected:
	virtual int onRead(WhSockHandle sock) = 0;
	virtual int onWrite(WhSockHandle sock) = 0;
	int waitForWritable(WhSockHandle sock);

	WhMutex mutexSocks_;
	WhCond cond_;

	fd_set readFds_;
	fd_set writeFds_;
	WhSockHandle maxFd_;

public:
	JySockSelector();
	~JySockSelector();

	void stop();
};

void setSockNonBlock(WhSockHandle sock);
void getLocalIpAndPort(WhSockHandle sock, string &ip, int &port);