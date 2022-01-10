#include <EuhatPreDef.h>
#include "JySockSelector.h"
#include <EuhatPostDef.h>

void setSockNonBlock(WhSockHandle sock)
{
#ifdef _WIN32  
	unsigned long on_windows = 1;
	if (ioctlsocket(sock, FIONBIO, &on_windows) < 0)
#else  
	/*	char on = 1;
	if (ioctlsocket(sock, FIONBIO, (char *)&on) < 0)
	*/
	int flags;
	flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0)
	{
		perror("fcntl(F_GETFL) failed");
		return;
	}
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
#endif
	{
		DBG(("set sock non blocking failed.\n"));
	}
}

void getLocalIpAndPort(WhSockHandle sock, string &ip, int &port)
{
	WhSockAddrIn recvAddr;
	WhSockLen recvAddrSize = sizeof(recvAddr);
	getsockname(sock, (WhSockAddr *)&recvAddr, &recvAddrSize);
	port = ntohs(recvAddr.sin_port);
	ip = inet_ntoa(recvAddr.sin_addr);
}

int JySockSelector::workProc()
{
	while (workThreadIsRunning_)
	{
		WhSockHandle maxFd;
		fd_set readFdsCopy;

		{
			WhMutexGuard guard(&mutexSocks_);
			readFdsCopy = readFds_;
			maxFd = maxFd_;
			if (maxFd == 0)
			{
				cond_.wait(&mutexSocks_);
				continue;
			}
		}

		fd_set readFds;

		readFds = readFdsCopy;

		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		int res = select((int)maxFd + 1, &readFds, NULL, NULL, &timeout);
		if (res == -1)
		{
			DBG(("select failed, last err: %d.\n", whGetLastError()));
			continue;
		}
		else if (res == 0)
		{
			//DBG(("no socket ready for read within %d secs.\n", 5));
			continue;
		}

#ifdef _WIN32
		for(unsigned int i = 0; i < readFdsCopy.fd_count; i++)
		{
			if (FD_ISSET(readFdsCopy.fd_array[i], &readFds))
			{
				onRead(readFdsCopy.fd_array[i]);
			}
		}
#else
		for (WhSockHandle i = 0; i < maxFd + 1; i++)
		{
			if (FD_ISSET(i, &readFds))
			{
				onRead(i);
			}
		}
#endif
	}

	return 1;
}

int JySockSelector::waitForWritable(WhSockHandle sock)
{
	while (workThreadIsRunning_)
	{
		fd_set writeFds;
		FD_ZERO(&writeFds);
		FD_SET(sock, &writeFds);

		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		int res = select((int)maxFd_ + 1, NULL, &writeFds, NULL, &timeout);
		if (res == -1)
		{
			DBG(("writing select failed, last err: %d.\n", whGetLastError()));
			continue;
		}
		else if (res == 0)
		{
			//DBG(("no socket ready for read within %d secs.\n", 5));
			continue;
		}

		if (FD_ISSET(sock, &writeFds))
		{
			return 1;
		}
	}
	return 0;
}

JySockSelector::JySockSelector()
{
	FD_ZERO(&readFds_);
	FD_ZERO(&writeFds_);
	maxFd_ = 0;
}

JySockSelector::~JySockSelector()
{
	stop();
}

void JySockSelector::stop()
{
	workThreadIsRunning_ = 0;
	cond_.signal();
	JyWorkThread::stop();
}