#include <EuhatPreDef.h>
#include "JyTcpSelector.h"
#include "JyTcpEndpoint.h"
#include <dbop/DbOpIni.h>
#include <time.h>
#include <EuhatPostDef.h>

JyTcpBuf::JyTcpBuf(JyBufPool &pool)
{
	pool.get(guard_);
	recvLen_ = 0;
}

JyTcpBuf::~JyTcpBuf()
{
}

JyTcpListener::JyTcpListener(JyTcpEndpoint *listener, JyTcpSelectType selectType)
{
	listener_ = listener;
	selectType_ = selectType;

	encClear_.reset(new JyDataEncryptClear());
	decClear_.reset(new JyDataDecryptClear());
	enc_ = encClear_.get();
	dec_ = decClear_.get();

	if (listener_->isSecure_)
	{
		encAsym_.reset(new JyDataEncryptAsymmetric());
		decAsym_.reset(new JyDataDecryptAsymmetric());
		encSym_.reset(new JyDataEncryptSymmetric());
		decSym_.reset(new JyDataDecryptSymmetric());

		DbOpIni ini(listener_->db_);

		string base = listener_->isServer_? JY_TCP_INI_SERVER_CERT_KEY : JY_TCP_INI_CLIENT_CERT_KEY;
		string key_n = base + "_n";
		JyBuf buf;
		ini.readBuf(JY_TCP_INI_CERT_SECTION, key_n.c_str(), buf);
		decAsym_->n_.setBuf(buf);
		string key_d = base + "_d";
		ini.readBuf(JY_TCP_INI_CERT_SECTION, key_d.c_str(), buf);
		decAsym_->d_.setBuf(buf);
		string key_e = base + "_e";
		ini.readBuf(JY_TCP_INI_CERT_SECTION, key_e.c_str(), e_);
	}
}

JyTcpListener::~JyTcpListener()
{
}

int JyTcpListener::hasWholePacket(unique_ptr<JyTcpBuf> &splitCompleted)
{
	if (recvBuf_->recvLen_ <= sizeof(int) + sizeof(short))
	{
		DBG(("packet is too short.\n"));
		return -1;
	}

	int totalSize = *(int *)recvBuf_->guard_.get();
	if (totalSize >= recvBuf_->guard_.pool_->getUnitSize())
	{
		DBG(("packet is too long.\n"));
		return -1;
	}

	if (recvBuf_->recvLen_ < totalSize)
		return 0;

	if (recvBuf_->recvLen_ == totalSize)
	{
		splitCompleted.reset(recvBuf_.release());
		return 1;
	}

	JyTcpBuf *nextBuf = new JyTcpBuf(*recvBuf_->guard_.pool_);
	nextBuf->recvLen_ = recvBuf_->recvLen_ - totalSize;
	memcpy(nextBuf->guard_.get(), recvBuf_->guard_.get() + totalSize, nextBuf->recvLen_);

	splitCompleted.reset(recvBuf_.release());
	splitCompleted->recvLen_ = totalSize;

	recvBuf_.reset(nextBuf);

	return 1;
}

JyTcpSelector::JyTcpSelector(JyBufPool *pool, JyTcpEndpoint *listener)
	: pool_(pool)
{
	listener_ = listener;
	totalSent_ = 0;
	totalRecv_ = 0;
	lastTotalSentRefresh_ = time(NULL);
	lastTotalRecvRefresh_ = time(NULL);
}

JyTcpSelector::~JyTcpSelector()
{
	for (auto &it : socks_)
	{
		whCloseSocket(it.first);
	}

	socks_.clear();
}

int JyTcpSelector::add(WhSockHandle sock, JyTcpSelectType selectType, JyTcpEndpoint *listener)
{
	setSockNonBlock(sock);

	WhMutexGuard guard(&mutexSocks_);

	FD_SET(sock, &writeFds_);
	FD_SET(sock, &readFds_);

	if (maxFd_ < sock)
		maxFd_ = sock;

	socks_[sock].reset(new JyTcpListener(listener, selectType));

	if (socks_.size() == 1)
		cond_.signal();

	return 1;
}

int JyTcpSelector::addRecv(WhSockHandle sock, JyTcpEndpoint *listener)
{
	return add(sock, JyTcpSelectRecv, listener);
}

int JyTcpSelector::addAccept(int port, JyTcpEndpoint *listener)
{
	WhSockHandle sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
	{
		perror("tcp setsockopt reuse addr error.\n");
		return 0;
	}

	setSockNonBlock(sock);

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons((u_short)port);
	if (::bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
		DBG(("tcp bind to port %d failed.\n", port));
		return 0;
	}
	
	add(sock, JyTcpSelectAccept, listener);

	::listen(sock, 10);

	return 1;
}

int JyTcpSelector::addConnect(const char *ip, int port, JyTcpEndpoint *listener)
{
	WhSockHandle sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
	{
		perror("tcp setsockopt reuse addr error.\n");
		return 0;
	}

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);

	DBG(("add connect listener %d.\n", sock));

	if (::connect(sock, (WhSockAddr *)&addr, sizeof(addr)) < 0)
	{
		DBG(("tcp connect peer failed. [%s:%d][%d]\n", ip, port, whGetLastError()));
		whCloseSocket(sock);
		listener->postMsg(JY_MSG_TYPE_SOCK_CONNECT_END, NULL, WH_INVALID_SOCKET);
		return 0;
	}

	setSockNonBlock(sock);

/*	int nRecvBuf = 32 * 1024;
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));

	int nSendBuf = 32 * 1024;
	setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char*)&nSendBuf, sizeof(int));
*/
	add(sock, JyTcpSelectRecv, listener);
	listener->postMsg(JY_MSG_TYPE_SOCK_CONNECT_END, NULL, sock);

	return 1;
}

int JyTcpSelector::send(WhSockHandle sock, JyBuf &buf)
{
	{
		WhMutexGuard guard(&mutexSocks_);

		auto it = socks_.find(sock);
		if (it == socks_.end())
			return 0;

		JyTcpListener *tcpL = it->second.get();
		tcpL->sendBuf_.reset(new JyBuf());
		tcpL->sendBuf_->data_.reset(buf.data_.release());
		tcpL->sendBuf_->size_ = buf.size_;
	}
	onWrite(sock);

	return 1;
}

int JyTcpSelector::del(WhSockHandle sock)
{
	WhMutexGuard guard(&mutexSocks_);

	auto it = socks_.find(sock);
	if (it == socks_.end())
		return 0;

	socks_.erase(it);

	whCloseSocket(sock);
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

int JyTcpSelector::onRead(WhSockHandle sock)
{
	WhMutexGuard guard(&mutexSocks_);

	auto it = socks_.find(sock);
	if (it == socks_.end())
	{
		DBG(("can't find reading sock.\n"));
		return 0;
	}

	if (it->second->selectType_ == JyTcpSelectAccept)
	{
		struct sockaddr_in clientAddress;
		WhSockLen clientLen = sizeof(clientAddress);
		int clientSock;
		clientSock = ::accept(sock, (struct sockaddr *)&clientAddress, &clientLen);
		if (WH_INVALID_SOCKET == clientSock)
		{
			DBG(("accept err: [%s].\n", strerror(errno)));
			return 0;
		}
		string peerIp = inet_ntoa(clientAddress.sin_addr);
		int peerPort = ntohs(clientAddress.sin_port);
		DBG(("tcp select accept [%s][%d].\n", peerIp.c_str(), peerPort));

		it->second->listener_->postMsg(JY_MSG_TYPE_SOCK_ACCEPT, NULL, clientSock);
		return 1;
	}

	if (NULL == it->second->recvBuf_.get())
		it->second->recvBuf_.reset(new JyTcpBuf(*pool_));

	int recvLen = recv(sock, it->second->recvBuf_->guard_.get() + it->second->recvBuf_->recvLen_, pool_->getUnitSize() - it->second->recvBuf_->recvLen_, 0);
	if (recvLen <= 0)
	{
		DBG(("tcp recv failed, errno is %d:[%s].\n", errno, strerror(errno)));
		it->second->recvBuf_.reset();
		it->second->listener_->postMsg(JY_MSG_TYPE_SOCK_DISCONNECT, NULL, sock);
		return 0;
	}
	totalRecvRefresh(recvLen);
	it->second->recvBuf_->recvLen_ += recvLen;

	unique_ptr<JyTcpBuf> outBuf;
	int result = it->second->hasWholePacket(outBuf);
	if (result == 1)
	{
		// this thread must be stopped before msgloop is stopped.
		// or else, here will happen memory leak.
		it->second->listener_->postMsg(JY_MSG_TYPE_SOCK_READ, outBuf.release(), sock);
	}
	else if (result == -1)
	{
		it->second->recvBuf_.reset();
		it->second->listener_->postMsg(JY_MSG_TYPE_SOCK_DISCONNECT, NULL, sock);
		return 0;
	}
	return 1;
}

int JyTcpSelector::onWrite(WhSockHandle sock)
{
	unique_ptr<JyBuf> buf;
	{
		WhMutexGuard guard(&mutexSocks_);

		auto it = socks_.find(sock);
		if (it == socks_.end())
		{
			DBG(("can't find writing sock.\n"));
			return 0;
		}

		buf.reset(it->second->sendBuf_.release());
	}

	int sentLen = 0;
	while (workThreadIsRunning_)
	{
		waitForWritable(sock);

		int len = ::send(sock, buf->data_.get() + sentLen, buf->size_ - sentLen, 0);
		if (len < 0)
		{
			DBG(("tcp recv failed, errno is %d:[%s].\n", errno, strerror(errno)));

			WhMutexGuard guard(&mutexSocks_);

			auto it = socks_.find(sock);
			if (it == socks_.end())
			{
				DBG(("can't find writing sock.\n"));
				return 0;
			}
			it->second->listener_->postMsg(JY_MSG_TYPE_SOCK_DISCONNECT, NULL, sock);
			return 0;
		}
		else
		{
			sentLen += len;
			totalSentRefresh(len);
			if (sentLen == buf->size_)
				break;
		}
	}

	return 1;
}

JyTcpListener *JyTcpSelector::getListener(WhSockHandle sock)
{
	WhMutexGuard guard(&mutexSocks_);

	auto it = socks_.find(sock);
	if (it == socks_.end())
	{
		return 0;
	}
	return it->second.get();
}

void JyTcpSelector::totalSentRefresh(int sent)
{
	return;
	totalSent_ += sent;
	time_t now = time(NULL);
	if (now - lastTotalSentRefresh_ > 3)
	{
		listener_->postMsg(JY_MSG_TYPE_SOCK_TOTAL_SEND, NULL, totalSent_);
		lastTotalSentRefresh_ = now;
		totalSent_ = 0;
		totalRecv_ = 0;
	}
}

void JyTcpSelector::totalRecvRefresh(int recved)
{
	return;
	totalRecv_ += recved;
	time_t now = time(NULL);
	if (now - lastTotalRecvRefresh_ > 3)
	{
		listener_->postMsg(JY_MSG_TYPE_SOCK_TOTAL_RECV, NULL, totalRecv_);
		lastTotalRecvRefresh_ = now;
		totalSent_ = 0;
		totalRecv_ = 0;
	}
}