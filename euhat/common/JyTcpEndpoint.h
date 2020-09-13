#pragma once

#include <common/JyMsgLoop.h>
#include <common/JyDataStream.h>

typedef int JyTcpPacketTypeType;

class JyTcpSelector;
class JyDataReadStream;
class JyDataWriteStream;
class DbOpSqlite;

class JyTcpEndpoint : public JyMsgLoop
{
	friend class JyTcpListener;

protected:
	int canSend(WhSockHandle sock);
	void writeHeader(JyDataWriteStream &ds, JyTcpPacketTypeType packetType);
	void send(WhSockHandle sock, JyDataWriteStream &ds);
	int onSockRead(WhSockHandle &sock, JyTcpPacketTypeType &packetType, unique_ptr<JyDataReadBlock> &ds, const JyMsg &msg);

	unique_ptr<JyBufPool> pool_;
	unique_ptr<JyTcpSelector> selector_;

	int isServer_;
	int isSecure_;
	DbOpSqlite *db_;

public:
	JyTcpEndpoint(int isServer, int poolUnitSize, int isSecure = 0);
	~JyTcpEndpoint();

	virtual void setDbOp(DbOpSqlite *db);
};