#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "EpClient.h"
#include <EuhatPostDef.h>

EpClient::EpClient(const char *serverIp, int serverPort, const char *id, const char *peerId)
{
	serverIp_ = serverIp;
	serverPort_ = serverPort;
	id_ = id;
	peerId_ = peerId;
}

EpClient::~EpClient()
{
	stop();
}

int EpClient::onInit(JyMsg &msg)
{
	whSockEnvInit();

	sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	int optval = 1;
	if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
	{
		perror("udp setsockopt reuse addr error.\n");
		return 0;
	}

	sendAddr_.sin_family = AF_INET;
	sendAddr_.sin_port = htons(serverPort_);
	sendAddr_.sin_addr.s_addr = inet_addr(serverIp_.c_str());

	recvAddr_.sin_family = AF_INET;
	recvAddr_.sin_port = 0;
	recvAddr_.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock_, (WhSockAddr *)&recvAddr_, sizeof(recvAddr_)) == -1)
	{
		DBG(("client bind error [%d].\n", errno));
		return 0;
	}

	WhSockLen recvAddrSize = sizeof(recvAddr_);
	getsockname(sock_, (WhSockAddr *)&recvAddr_, &recvAddrSize);
	localPort_ = ntohs(recvAddr_.sin_port);
	localIp_ = inet_ntoa(recvAddr_.sin_addr);
	DBG(("local addr is %s:%d.\n", localIp_.c_str(), localPort_));

	selector_.start();
	selector_.add(sock_, this);

	postMsg(EPCLIENT_CMD_ALLOC);

	return 1;
}

int EpClient::onFini(JyMsg &msg)
{
	selector_.del(sock_);

	whCloseSocket(sock_);

	whSockEnvFini();

	return 1;
}

int EpClient::onAlloc(JyMsg &msg)
{
	string cmd = "alloc";
	cmd = cmd + "\n" + localIp_ + "\n" + intToString(localPort_) + "\n" + id_;

	selector_.send(sock_, sendAddr_, cmd.c_str());

	delayWork(EPCLIENT_CMD_ALLOC, 10 * 1000);
	return 1;
}

int EpClient::onConnect(JyMsg &msg)
{
	string cmd = "connect";
	cmd = cmd + "\n" + peerId_;

	selector_.send(sock_, sendAddr_, cmd.c_str());

	return 1;
}

int EpClient::onPeerCallTcpSrv(JyMsg &msg)
{
	WhSockHandle sockTcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int optval = 1;
	if (setsockopt(sockTcp, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
	{
		perror("tcp setsockopt reuse addr error.\n");
		return 0;
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons((u_short)localPort_);
	if (bind(sockTcp, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
		DBG(("tcp bind to port %d failed.\n", localPort_));
		return 0;
	}

	listen(sockTcp, 10);
	selector_.del(sock_);
	whCloseSocket(sock_);
	sock_ = WH_INVALID_SOCKET;

	sockaddr_in addr;
	WhSockLen addrLen = sizeof(addr);
	WhSockHandle sockCli = accept(sockTcp, (WhSockAddr *)&addr, &addrLen);
	if (WH_INVALID_SOCKET == sockCli)
	{
		DBG(("accept failed.\n"));
		return 0;
	}

	string cliIp = inet_ntoa(addr.sin_addr);
	int cliPort = ntohs(addr.sin_port);
	DBG(("accept from [%s:%d]\n", cliIp.c_str(), cliPort));

	int i = 0;
	while (1)
	{
		char buf[1024];
		sprintf(buf, "hello, [%d]", i++);
		send(sockCli, buf, (int)strlen(buf) + 1, 0);
		int recvLen = recv(sockCli, buf, sizeof(buf) - 1, 0);
		if (recvLen > 0)
		{
			buf[sizeof(buf) - 1] = 0;
			DBG(("receive from peer: [%s]\n", buf));
		}
		else
		{
			DBG(("receive failed.\n"));
		}
	}

	whCloseSocket(sockTcp);
	return 1;
}

int EpClient::onPeerCallTcpCli(JyMsg &msg)
{
	WhSockHandle sockTcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int optval = 1;
	if (setsockopt(sockTcp, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval)) < 0)
	{
		perror("tcp setsockopt reuse addr error.\n");
		return 0;
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons((u_short)localPort_);
	if (bind(sockTcp, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
	{
		DBG(("tcp bind to port %d failed.\n", localPort_));
		return 0;
	}

//	listen(sockTcp, 10);

	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(peer_.natPort_);
	addr.sin_addr.s_addr = inet_addr(peer_.natIp_.c_str());

	while (1)
	{
		if (connect(sockTcp, (WhSockAddr *)&addr, sizeof(addr)) < 0)
		{
			DBG(("tcp connect peer failed. [%s:%d]\n", peer_.natIp_.c_str(), peer_.natPort_));
			whSleep(5);
		}
		else
			break;
	}

	int i = 0;
	while (1)
	{
		char buf[1024];
		sprintf(buf, "hello, [%d]", i++);
		send(sockTcp, buf, (int)strlen(buf) + 1, 0);
		int recvLen = recv(sockTcp, buf, sizeof(buf) - 1, 0);
		if (recvLen > 0)
		{
			buf[sizeof(buf) - 1] = 0;
			DBG(("receive from peer: [%s]\n", buf));
		}
		else
		{
			DBG(("receive failed.\n"));
		}
	}

	whCloseSocket(sockTcp);
	return 1;
}

int EpClient::onPeerCall(JyMsg &msg)
{
	if (id_ == "euhat")
		return onPeerCallTcpSrv(msg);
	else
		return onPeerCallTcpCli(msg);

	string cmd = "hello";
	cmd = cmd + "\n" + "inner";

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(peer_.innerPort_);
	addr.sin_addr.s_addr = inet_addr(peer_.innerIp_.c_str());

	selector_.send(sock_, addr, cmd.c_str());

	cmd = "hello";
	cmd = cmd + "\n" + "nat";

	addr.sin_family = AF_INET;
	addr.sin_port = htons(peer_.natPort_);
	addr.sin_addr.s_addr = inet_addr(peer_.natIp_.c_str());
	DBG(("send to peer nat [%s:%d].\n", peer_.natIp_.c_str(), peer_.natPort_));

	selector_.send(sock_, addr, cmd.c_str());

	cmd = "relay";
	cmd = cmd + "\n" + id_ + "\n" + peer_.natIp_ + "\n" + intToString(peer_.natPort_);

	selector_.send(sock_, sendAddr_, cmd.c_str());

	delayWork(EPCLIENT_CMD_PEER_CALL, 10 * 1000);
	return 1;
}

int EpClient::onSockRead(JyMsg &msg)
{
	unique_ptr<JyUdpRecvBuf> buf((JyUdpRecvBuf *)msg.p_);
	WhSockHandle sock = (WhSockHandle)msg.int_;
	if (sock != sock_)
	{
		DBG(("received a unknown sock.\n"));
		return 0;
	}

	vector<string> tokens;
	splitTokenString(buf->buf_, '\n', tokens);

	if (tokens.size() < 2)
	{
		DBG(("[%s] return token count is less than 2.\n", tokens[0].c_str()));
		return 0;
	}

	string ip = inet_ntoa(buf->recvAddr_.sin_addr);
	int port = ntohs(buf->recvAddr_.sin_port);

	string cmd = tokens[0];
	string reply = tokens[1];

	tokens.erase(tokens.begin());
	tokens.erase(tokens.begin());

	if (cmd == "alloc")
	{
		if (reply != "ok")
			DBG(("alloc return [%s]\n", reply.c_str()));
		else
		{
			cancelWork(EPCLIENT_CMD_ALLOC);
			postMsg(EPCLIENT_CMD_CONNECT);
		}
		return 1;
	}
	if (cmd == "connect")
	{
		if (reply != "peer")
			DBG(("connect return [%s]\n", reply.c_str()));
		else
		{
			if (tokens.size() != 5)
				DBG(("connect return tokens count is not correct.\n"));
			else
			{
				peer_.id_ = tokens[0];
				peer_.natIp_ = tokens[1];
				peer_.natPort_ = atoi(tokens[2].c_str());
				peer_.innerIp_ = tokens[3];
				peer_.innerPort_ = atoi(tokens[4].c_str());

				cancelWork(EPCLIENT_CMD_CONNECT);
				postMsg(EPCLIENT_CMD_PEER_CALL);
				return 1;
			}
		}
		delayWork(EPCLIENT_CMD_CONNECT, 10 * 1000);
		return 1;
	}
	if (cmd == "hello")
	{
		DBG(("hello reply [%s] from [%s:%d]\n", reply.c_str(), ip.c_str(), port));
		return 1;
	}
	if (cmd == "relay")
	{
		DBG(("relay [%s] from [%s:%d]\n", reply.c_str(), ip.c_str(), port));
		return 1;
	}

	return 1;
}

int EpClient::onTimer(JyMsg &msg)
{
	int id = msg.int_;

	return JyMsgLoop::onTimer(msg);
}

int EpClient::onWork(JyMsg &msg)
{
	switch (msg.cmd_)
	{
	case JY_MSG_TYPE_INIT: return onInit(msg);
	case JY_MSG_TYPE_FINI: return onFini(msg);
	case JY_MSG_TYPE_TIMER: return onTimer(msg);
	case EPCLIENT_CMD_ALLOC: return onAlloc(msg);
	case EPCLIENT_CMD_CONNECT: return onConnect(msg);
	case EPCLIENT_CMD_PEER_CALL: return onPeerCall(msg);
	case JY_MSG_TYPE_SOCK_READ: return onSockRead(msg);
	default:
		break;
	}
	return 1;
}