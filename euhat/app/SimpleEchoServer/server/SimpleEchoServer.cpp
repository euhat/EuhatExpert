#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <common/JyTcpSelector.h>
#include "SimpleEchoServer.h"
#include <EuhatPostDef.h>

SimpleEchoServer::SimpleEchoServer(int serverPort)
	: JyTcpServer(serverPort, 1024 * 1024 * 20)
{

}

SimpleEchoServer::~SimpleEchoServer()
{
	stop();
}

void SimpleEchoServer::onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds)
{
	string msg = ds->getStr();

	DBG(("receive client [%s]\n", msg.c_str()));

	JyDataWriteStream dsOut;
	writeHeader(dsOut, SES_CMD_ECHO);
	dsOut.putStr("this is server send msg.\n");

	send(sock, dsOut);
}
