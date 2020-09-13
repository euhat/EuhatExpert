#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <common/JyDataStream.h>
#include "JyTcpSelector.h"
#include "JyTcpEndpoint.h"
#include <dbop/DbOpIni.h>
#include <EuhatPostDef.h>

JyTcpEndpoint::JyTcpEndpoint(int isServer, int poolUnitSize, int isSecure)
{
	isServer_ = isServer;
	isSecure_ = isSecure;

	pool_.reset(new JyBufPool(poolUnitSize, 10));
	selector_.reset(new JyTcpSelector(pool_.get(), this));
}

JyTcpEndpoint::~JyTcpEndpoint()
{

}

void JyTcpEndpoint::setDbOp(DbOpSqlite *db)
{
	db_ = db;
}

int JyTcpEndpoint::canSend(WhSockHandle sock)
{
	if (NULL == selector_.get())
		return 0;
	if (WH_INVALID_SOCKET == sock)
		return 0;
	return 1;
}

void JyTcpEndpoint::writeHeader(JyDataWriteStream &ds, JyTcpPacketTypeType packetType)
{
	ds.put<int>(0);
	ds.put<JyTcpPacketTypeType>(packetType);
}

void JyTcpEndpoint::send(WhSockHandle sock, JyDataWriteStream &ds)
{
	if (!canSend(sock))
		return;

	if (WH_INVALID_SOCKET == sock)
		return;

	JyTcpListener *l = selector_->getListener(sock);

	char *p = ds.buf_.data_.get();

	if (l->enc_ == l->encClear_.get())
	{
		*(int *)p = ds.size();

		ds.buf_.size_ = ds.size();

		selector_->send(sock, ds.buf_);
		return;
	}

	JyDataWriteStream dsOut;

	dsOut.put((int)0);
	dsOut.put(*(JyTcpPacketTypeType *)(p + sizeof(int)));

	l->enc_->encrypt(dsOut, p + sizeof(int) + sizeof(JyTcpPacketTypeType), ds.size() - sizeof(int) - sizeof(JyTcpPacketTypeType));

	p = dsOut.buf_.data_.get();
	*(int *)p = dsOut.size();

	dsOut.buf_.size_ = dsOut.size();

	ds.buf_.reset();

	selector_->send(sock, dsOut.buf_);
}

int JyTcpEndpoint::onSockRead(WhSockHandle &sock, JyTcpPacketTypeType &packetType, unique_ptr<JyDataReadBlock> &ds, const JyMsg &msg)
{
	unique_ptr<JyTcpBuf> buf((JyTcpBuf *)msg.p_);
	sock = (WhSockHandle)msg.int_;

	if (!canSend(sock))
		return 0;

	JyTcpListener *l = selector_->getListener(sock);

	if (l->dec_ == l->decClear_.get())
	{
		ds.reset(new JyDataReadBlock(*buf->guard_.pool_, buf->guard_.release(), buf->recvLen_));

		int packetLen = ds->get<int>();
		packetType = ds->get<JyTcpPacketTypeType>();

		return 1;
	}

	packetType = *(JyTcpPacketTypeType *)(buf->guard_.get() + sizeof(int));

	JyDataWriteBlock dsOut(*buf->guard_.pool_);
	if (!l->dec_->decrypt(dsOut, buf->guard_.get() + sizeof(int) + sizeof(JyTcpPacketTypeType), buf->recvLen_ - sizeof(int) - sizeof(JyTcpPacketTypeType)))
	{
		postMsg(JY_MSG_TYPE_SOCK_DISCONNECT, NULL, sock);
		return 0;
	}

	ds.reset(new JyDataReadBlock(*buf->guard_.pool_, dsOut.guard_.release(), dsOut.size()));

	buf.reset();

	return 1;
}