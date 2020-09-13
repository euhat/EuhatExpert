#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "JyTcpSelector.h"
#include "JyTcpClient.h"
#include <common/JyDataEncrypt.h>
#include <dbop/DbOpIni.h>
#include <time.h>
#include <EuhatPostDef.h>

JyTcpClient::JyTcpClient(const char *serverIp, int port, int poolUnitSize, int isSecure)
	: JyTcpEndpoint(0, poolUnitSize, isSecure)
{
	serverIp_ = serverIp;
	serverPort_ = port;
}

JyTcpClient::~JyTcpClient()
{

}

void JyTcpClient::onDisconnect(JyMsg &msg)
{
	WhSockHandle sock = (WhSockHandle)msg.int_;

	selector_->del(sock);

	sock_ = WH_INVALID_SOCKET;
}

void JyTcpClient::onInit(JyMsg &msg)
{
	whSockEnvInit();

	selector_->start();

	postMsg(JY_MSG_TYPE_SOCK_CONNECT_BEGIN);
}

void JyTcpClient::onFini(JyMsg &msg)
{
	selector_->stop();
	selector_.reset();

	whSockEnvFini();
}

void JyTcpClient::onConnectBegin(JyMsg &msg)
{
	selector_->addConnect(serverIp_.c_str(), serverPort_, this);
}

void JyTcpClient::onConnectEnd(JyMsg &msg)
{
	WhSockHandle sock = (WhSockHandle)msg.int_;
	if (WH_INVALID_SOCKET == sock)
	{
		if (NULL == selector_.get())
			return;

		whSleep(1);
		postMsg(JY_MSG_TYPE_SOCK_CONNECT_BEGIN);
		return;
	}
	sock_ = sock;

	if (isSecure_)
	{
		postMsg(JY_MSG_TYPE_SOCK_EXCHANGE_ASYM_SECURITY);
	}
	else
	{
		postMsg(JY_MSG_TYPE_USER_START);
	}
}

void JyTcpClient::onExchangeAsymSecurity(JyMsg &msg)
{
	if (!canSend(sock_))
		return;

	JyTcpListener *l = selector_->getListener(sock_);

	JyDataWriteStream ds;
	writeHeader(ds, JyTcpCmdExchangeAsymSecurity);
	JyBuf buf;
	l->decAsym_->n_.getBuf(buf);
	ds.putBuf(buf.data_.get(), buf.size_);
	ds.putBuf(l->e_.data_.get(), l->e_.size_);

	send(sock_, ds);

	WhTimeVal val;
	val.now();
	srand((unsigned int)val.t_.tv_usec);
}

void JyTcpClient::onReadExchangeAsymSecurity(unique_ptr<JyDataReadBlock> &ds)
{
	if (!canSend(sock_))
		return;

	JyTcpListener *l = selector_->getListener(sock_);

	JyBuf buf;
	ds->getBuf(buf);
	if (buf.size_ < 10)
	{
		DBG(("receive certificate length is too short, close the socket.\n"));
		postMsg(JY_MSG_TYPE_SOCK_DISCONNECT, NULL, sock_);
		return;
	}
	l->encAsym_->n_.setBuf(buf);
	ds->getBuf(buf);
	l->encAsym_->e_.setBuf(buf);

	l->enc_ = l->encAsym_.get();
	l->dec_ = l->decAsym_.get();

	postMsg(JY_MSG_TYPE_SOCK_EXCHANGE_SYM_SECURITY);
}

void JyTcpClient::onExchangeSymSecurity(JyMsg &msg)
{
	if (!canSend(sock_))
		return;

	JyTcpListener *l = selector_->getListener(sock_);

	JyDataWriteStream ds;
	writeHeader(ds, JyTcpCmdExchangeSymSecurity);
#if 0
	JyBigNumCtx ctx;
	JyBigNum bn(ctx);
	bn.rand(2048);
	JyBuf buf;
	bn.getBuf(buf);
	ds.putBuf(buf.data_.get(), buf.size_);
	l->decSym_->xor_.reset(buf);

	bn.rand(2048);
	bn.getBuf(buf);
	ds.putBuf(buf.data_.get(), buf.size_);
	l->encSym_->xor_.reset(buf);
#else
	JyBuf buf(2048);
	whRandUniformBuf(buf);
	ds.putBuf(buf.data_.get(), buf.size_);
	l->decSym_->xor_.reset(buf);

	buf.reset(2048);
	whRandUniformBuf(buf);
	ds.putBuf(buf.data_.get(), buf.size_);
	l->encSym_->xor_.reset(buf);
#endif

	ds.putStr(visitCode_.c_str());

	send(sock_, ds);
}

void JyTcpClient::onReadExchangeSymSecurity(unique_ptr<JyDataReadBlock> &ds)
{
	if (!canSend(sock_))
		return;
	if (NULL == ds.get())
		return;

	JyTcpListener *l = selector_->getListener(sock_);

	l->enc_ = l->encSym_.get();
	l->dec_ = l->decSym_.get();

	postMsg(JY_MSG_TYPE_USER_START);
}

void JyTcpClient::onUserStart(JyMsg &msg)
{

}

void JyTcpClient::onUserLoop(JyMsg &msg)
{

}

void JyTcpClient::onSockRead(JyMsg &msg)
{
	WhSockHandle sock;
	JyTcpPacketTypeType packetType;
	unique_ptr<JyDataReadBlock> ds;

	if (!JyTcpEndpoint::onSockRead(sock, packetType, ds, msg))
		return;

	if (packetType == JyTcpCmdExchangeAsymSecurity)
	{
		onReadExchangeAsymSecurity(ds);
	}
	else if (packetType == JyTcpCmdExchangeSymSecurity)
	{
		onReadExchangeSymSecurity(ds);
	}
	else
	{
		onRead(packetType, ds);
	}
}

void JyTcpClient::onRead(short packetType, unique_ptr<JyDataReadBlock> &ds)
{

}

int JyTcpClient::onWork(JyMsg &msg)
{
	switch (msg.cmd_)
	{
	case JY_MSG_TYPE_USER_LOOP: onUserLoop(msg); return 1;
	case JY_MSG_TYPE_USER_START: onUserStart(msg); return 1;
	case JY_MSG_TYPE_SOCK_READ: onSockRead(msg); return 1;
	case JY_MSG_TYPE_SOCK_CONNECT_BEGIN: onConnectBegin(msg); return 1;
	case JY_MSG_TYPE_SOCK_CONNECT_END: onConnectEnd(msg); return 1;
	case JY_MSG_TYPE_SOCK_EXCHANGE_ASYM_SECURITY: onExchangeAsymSecurity(msg); return 1;
	case JY_MSG_TYPE_SOCK_EXCHANGE_SYM_SECURITY: onExchangeSymSecurity(msg); return 1;
	case JY_MSG_TYPE_SOCK_DISCONNECT: onDisconnect(msg); return 1;
	case JY_MSG_TYPE_SOCK_TOTAL_SEND: onTotalSend(msg); return 1;
	case JY_MSG_TYPE_SOCK_TOTAL_RECV: onTotalRecv(msg); return 1;
	case JY_MSG_TYPE_INIT: onInit(msg); return 1;
	case JY_MSG_TYPE_FINI: onFini(msg); return 1;
	case JY_MSG_TYPE_TIMER: onTimer(msg); return 1;
	}
	return 1;
}