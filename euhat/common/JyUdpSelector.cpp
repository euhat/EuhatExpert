#include <EuhatPreDef.h>
#include "JyUdpSelector.h"
#include "JyMsgLoop.h"
#include <EuhatPostDef.h>

#define JY_UDP_RECV_UNIT_LEN 1024
#define JY_UDP_SEND_UNIT_LEN 1024

JyUdpRecvBuf::JyUdpRecvBuf()
{
	buf_ = (char *)malloc(JY_UDP_RECV_UNIT_LEN);
	bufLen_ = JY_UDP_RECV_UNIT_LEN;
}

JyUdpRecvBuf::~JyUdpRecvBuf()
{
	free(buf_);
}

JyUdpSendBuf::JyUdpSendBuf()
{
	buf_ = (char *)malloc(JY_UDP_SEND_UNIT_LEN);
	bufLen_ = JY_UDP_SEND_UNIT_LEN;
}

JyUdpSendBuf::~JyUdpSendBuf()
{
	free(buf_);
}

JyUdpListener::JyUdpListener(JyMsgLoop *listener)
{
	listener_ = listener;
}

JyUdpListener::~JyUdpListener()
{
}

JyUdpSelector::~JyUdpSelector()
{
	WhMutexGuard guard(&mutexSocks_);

	for (auto &it : socks_)
	{
		it.second.reset();
	}
}

WhSockHandle JyUdpSelector::bind(int port)
{
	WhSockHandle sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
	{
		perror("udp setsockopt reuse addr error.\n");
		return 0;
	}

	setSockNonBlock(sock);

	WhSockAddrIn recvAddr;
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons((u_short)port);
	recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (::bind(sock, (WhSockAddr *)&recvAddr, sizeof(recvAddr)) == -1)
	{
		DBG(("client bind error [%d].\n", errno));
		return sock;
	}
	return sock;
}

int JyUdpSelector::add(WhSockHandle sock, JyMsgLoop *listener)
{
	setSockNonBlock(sock);

	WhMutexGuard guard(&mutexSocks_);

	FD_SET(sock, &writeFds_);
	FD_SET(sock, &readFds_);

	if (maxFd_ < sock)
		maxFd_ = sock;

	socks_[sock].reset(new JyUdpListener(listener));

	if (socks_.size() == 1)
		cond_.signal();

	return 1;
}

int JyUdpSelector::send(WhSockHandle sock, unique_ptr<JyUdpSendBuf> &buf)
{
	{
		WhMutexGuard guard(&mutexSocks_);

		auto it = socks_.find(sock);
		if (it == socks_.end())
			return 0;

		JyUdpListener *udpL = it->second.get();
		udpL->sendQueue_.push_back(move(buf));
	}
	onWrite(sock);

	return 1;
}

int JyUdpSelector::send(WhSockHandle sock, WhSockAddrIn &sockAddr, const char *buf, int bufLen)
{
	unique_ptr<JyUdpSendBuf> sendBuf(new JyUdpSendBuf);
	sendBuf->sendAddr_ = sockAddr;
	if (bufLen >= sendBuf->bufLen_)
	{
		DBG(("buf len is too big [%d], can't send.\n", bufLen));
		return 0;
	}
	memcpy(sendBuf->buf_, buf, bufLen);
	sendBuf->sendLen_ = bufLen;

	return send(sock, sendBuf);
}

int JyUdpSelector::send(WhSockHandle sock, WhSockAddrIn &sockAddr, const char *buf)
{
	return send(sock, sockAddr, buf, (int)strlen(buf) + 1);
}

int JyUdpSelector::del(WhSockHandle sock)
{
	WhMutexGuard guard(&mutexSocks_);

	auto it = socks_.find(sock);
	if (it == socks_.end())
		return 0;

	socks_.erase(it);

	FD_CLR(sock, &readFds_);
	FD_CLR(sock, &writeFds_);

	maxFd_ = 0;
	for (auto &it : socks_)
	{
		if (maxFd_ < it.first)
			maxFd_ = it.first;
	}

	return 1;
}

int JyUdpSelector::onRead(WhSockHandle sock)
{
	unique_ptr<JyUdpRecvBuf> buf(new JyUdpRecvBuf);

	WhSockLen recvAddrSize = sizeof(buf->recvAddr_);
	buf->recvLen_ = recvfrom(sock, buf->buf_, buf->bufLen_ - 1, 0, (WhSockAddr *)&buf->recvAddr_, &recvAddrSize);
	if (buf->recvLen_ < 0)
	{
		DBG(("udp recv failed.\n"));
		return 0;
	}
	buf->buf_[buf->recvLen_] = 0;

	WhMutexGuard guard(&mutexSocks_);

	auto it = socks_.find(sock);
	if (it == socks_.end())
	{
		DBG(("can't find reading sock.\n"));
		return 0;
	}

	it->second->listener_->postMsg(JY_MSG_TYPE_SOCK_READ, buf.release(), (int)sock);
	return 1;
}

int JyUdpSelector::onWrite(WhSockHandle sock)
{
	unique_ptr<JyUdpSendBuf> buf;
	{
		WhMutexGuard guard(&mutexSocks_);

		auto it = socks_.find(sock);
		if (it == socks_.end())
		{
			DBG(("can't find writing sock.\n"));
			return 0;
		}

		if (it->second->sendQueue_.size() == 0)
			return 1;
		buf.reset(it->second->sendQueue_.front().release());
		it->second->sendQueue_.pop_front();
	}

	waitForWritable(sock);

	sendto(sock, buf->buf_, buf->sendLen_, 0, (WhSockAddr *)&buf->sendAddr_, sizeof(buf->sendAddr_));

	return 1;
}