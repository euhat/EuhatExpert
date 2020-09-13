#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <common/JyDataEncrypt.h>
#include "JyTcpSelector.h"
#include "JyTcpServer.h"
#include <EuhatPostDef.h>

JyTcpServer::JyTcpServer(int serverPort, int poolUnitSize, int isSecure)
	: JyTcpEndpoint(1, poolUnitSize, isSecure),
	  serverPort_(serverPort)
{

}

JyTcpServer::~JyTcpServer()
{

}

void JyTcpServer::onInit(JyMsg &msg)
{
	whSockEnvInit();

	selector_->start();

	postMsg(JY_MSG_TYPE_SOCK_LISTEN);
}

void JyTcpServer::onFini(JyMsg &msg)
{
	DBG(("~JyTcpServer call in.\n"));
	selector_->stop();
	selector_.reset();
	DBG(("~JyTcpServer after selector stop.\n"));

	whSockEnvFini();
}

void JyTcpServer::onListen(JyMsg &msg)
{
	if (NULL == selector_.get())
		return;

	if (!selector_->addAccept(serverPort_, this))
		postMsg(JY_MSG_TYPE_SOCK_LISTEN);
}

void JyTcpServer::onSockAccept(JyMsg &msg)
{
	if (NULL == selector_.get())
		return;

	WhSockHandle sock = (WhSockHandle)msg.int_;

	selector_->addRecv(sock, this);
}

void JyTcpServer::onDisconnect(JyMsg &msg)
{
	if (NULL == selector_.get())
		return;

	WhSockHandle sock = (WhSockHandle)msg.int_;

	selector_->del(sock);
}

void JyTcpServer::onReadExchangeAsymSecurity(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	if (!canSend(sock))
		return;

	JyTcpListener *l = selector_->getListener(sock);

	JyBuf buf;
	ds->getBuf(buf);
	l->encAsym_->n_.setBuf(buf);
	ds->getBuf(buf);
	l->encAsym_->e_.setBuf(buf);

	JyDataWriteStream dsAck;
	writeHeader(dsAck, JyTcpCmdExchangeAsymSecurity);
	l->decAsym_->n_.getBuf(buf);
	dsAck.putBuf(buf.data_.get(), buf.size_);
	dsAck.putBuf(l->e_.data_.get(), l->e_.size_);

	send(sock, dsAck);

	l->enc_ = l->encAsym_.get();
	l->dec_ = l->decAsym_.get();
}

void JyTcpServer::onWriteHostInfo(JyDataWriteStream &dsAck)
{}

void JyTcpServer::onReadExchangeSymSecurity(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	if (!canSend(sock))
		return;
	if (NULL == ds.get())
		return;

	JyTcpListener *l = selector_->getListener(sock);

	ds->getBuf(l->encSym_->xor_);
	ds->getBuf(l->decSym_->xor_);

	char *visitCode = ds->getStr();
	if (visitCode_ != visitCode)
	{
		DBG(("visit code mismatch, will close client.\n"));

		JyBuf buf(2048);
		whRandUniformBuf(buf);
		l->decSym_->xor_.reset(buf);
	}

	JyDataWriteStream dsAck;
	writeHeader(dsAck, JyTcpCmdExchangeSymSecurity);
	dsAck.put<int>(visitCode_ == visitCode);
	onWriteHostInfo(dsAck);

	send(sock, dsAck);

	l->enc_ = l->encSym_.get();
	l->dec_ = l->decSym_.get();
}

void JyTcpServer::onSockRead(JyMsg &msg)
{
	WhSockHandle sock;
	JyTcpPacketTypeType packetType;
	unique_ptr<JyDataReadBlock> ds;

	if (!JyTcpEndpoint::onSockRead(sock, packetType, ds, msg))
		return;

	if (packetType == JyTcpCmdExchangeAsymSecurity)
	{
		onReadExchangeAsymSecurity(sock, ds);
	}
	else if (packetType == JyTcpCmdExchangeSymSecurity)
	{
		onReadExchangeSymSecurity(sock, ds);
	}
	else
	{
		onRead(sock, packetType, ds);
	}
}

void JyTcpServer::onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds)
{

}

int JyTcpServer::onWork(JyMsg &msg)
{
	switch (msg.cmd_)
	{
	case JY_MSG_TYPE_SOCK_READ: onSockRead(msg); return 1;
	case JY_MSG_TYPE_INIT: onInit(msg); return 1;
	case JY_MSG_TYPE_FINI: onFini(msg); return 1;
	case JY_MSG_TYPE_SOCK_LISTEN: onListen(msg); return 1;
	case JY_MSG_TYPE_SOCK_ACCEPT: onSockAccept(msg); return 1;
	case JY_MSG_TYPE_SOCK_DISCONNECT: onDisconnect(msg); return 1;
	}
	return 1;
}