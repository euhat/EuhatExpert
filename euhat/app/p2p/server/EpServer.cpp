#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "EpServer.h"
#include <EuhatPostDef.h>

EpServer::EpServer(int port)
{
	port_ = port;
}

EpServer::~EpServer()
{

}

int EpServer::onInit(JyMsg &msg)
{
	whSockEnvInit();

	sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sendAddr_.sin_family = AF_INET;
	sendAddr_.sin_port = htons(port_);
	sendAddr_.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock_, (WhSockAddr *)&sendAddr_, sizeof(sendAddr_)) == -1)
	{
		DBG(("server bind error [%d].\n", errno));
		return 0;
	}

	sockRelay_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	sendAddr_.sin_family = AF_INET;
	sendAddr_.sin_port = htons(port_ - 1);
	sendAddr_.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockRelay_, (WhSockAddr *)&sendAddr_, sizeof(sendAddr_)) == -1)
	{
		DBG(("server bind error [%d].\n", errno));
		return 0;
	}

	DBG(("server bind port %d successfully.\n", port_));

	selector_.start();
	selector_.add(sock_, this);
	selector_.add(sockRelay_, this);

	return 1;
}

int EpServer::onFini(JyMsg &msg)
{
	selector_.del(sockRelay_);
	whCloseSocket(sockRelay_);

	selector_.del(sock_);
	whCloseSocket(sock_);

	whSockEnvFini();

	return 1;
}

int EpServer::onSockRead(JyMsg &msg)
{
	unique_ptr<JyUdpRecvBuf> buf((JyUdpRecvBuf *)msg.p_);

	vector<string> tokens;
	splitTokenString(buf->buf_, '\n', tokens);

	string retStr;

	if (tokens[0] == "alloc")
	{
		if (tokens.size() != 4)
			retStr = "failed";
		else
		{
			string id = tokens[3];
			auto it = find_if(clients_.begin(), clients_.end(), [id](EpClientEntity &entity) { return entity.id_ == id; });
			if (it != clients_.end())
				clients_.erase(it);

			clients_.push_back(EpClientEntity());
			EpClientEntity &entity = clients_.back();
			entity.innerIp_ = tokens[1];
			entity.innerPort_ = atoi(tokens[2].c_str());
			entity.natIp_ = inet_ntoa(buf->recvAddr_.sin_addr);
			entity.natPort_ = ntohs(buf->recvAddr_.sin_port);
			entity.id_ = id;

			DBG(("%s's nat:%s:%d\n", entity.id_.c_str(), entity.natIp_.c_str(), entity.natPort_));
			retStr = "ok";
		}
	}
	else if (tokens[0] == "connect")
	{
		if (tokens.size() != 2)
			retStr = "failed";
		else
		{
			string ip = inet_ntoa(buf->recvAddr_.sin_addr);
			int port = ntohs(buf->recvAddr_.sin_port);
			auto itSelf = find_if(clients_.begin(), clients_.end(), [ip, port](EpClientEntity &entity) { return ip == entity.natIp_ && port == entity.natPort_; });
			if (itSelf == clients_.end())
				retStr = "not register";
			else
			{
				string id = tokens[1];
				auto it = find_if(clients_.begin(), clients_.end(), [id](EpClientEntity &entity) { return entity.id_ == id; });
				if (it == clients_.end())
					retStr = "not exist";
				else
				{
					retStr = "peer";
					retStr = retStr + "\n" + id + "\n" + 
						it->natIp_ + "\n" + intToString(it->natPort_) + "\n" + 
						it->innerIp_ + "\n" + intToString(it->innerPort_);
				}
			}
		}
	}
	else if (tokens[0] == "relay")
	{
		if (tokens.size() != 4)
			retStr = "failed";
		else
		{
			string ip = inet_ntoa(buf->recvAddr_.sin_addr);
			int port = ntohs(buf->recvAddr_.sin_port);
			string fromId = tokens[1];
			string toIp = tokens[2];
			int toPort = atoi(tokens[3].c_str());

			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(toPort);
			addr.sin_addr.s_addr = inet_addr(toIp.c_str());

			char hello[1024];
			sprintf(hello, "from [%s:%d,%s] to [%s:%d], hello.", ip.c_str(), port, fromId.c_str(), toIp.c_str(), toPort);
			retStr = tokens[0] + "\n" + hello;
			selector_.send(sockRelay_, addr, retStr.c_str());
			return 1;
		}
	}

	retStr = tokens[0] + "\n" + retStr;
	selector_.send(sock_, buf->recvAddr_, retStr.c_str());

	return 1;
}

int EpServer::onTimer(JyMsg &msg)
{
	return JyMsgLoop::onTimer(msg);
}

int EpServer::onWork(JyMsg &msg)
{
	switch (msg.cmd_)
	{
	case JY_MSG_TYPE_INIT: return onInit(msg);
	case JY_MSG_TYPE_FINI: return onFini(msg);
	case JY_MSG_TYPE_TIMER: return onTimer(msg);
	case JY_MSG_TYPE_SOCK_READ: return onSockRead(msg);
	default:
		break;
	}
	return 1;
}
