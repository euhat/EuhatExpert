#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include <common/JyTcpSelector.h>
#include "SimpleEchoClient.h"
#include <EuhatPostDef.h>

SimpleEchoClient::SimpleEchoClient(const char *serverIp, int serverPort)
	: JyTcpClient(serverIp, serverPort, 1024 * 1024 * 20)
{

}

SimpleEchoClient::~SimpleEchoClient()
{
	stop();
}

void SimpleEchoClient::onUserStart(JyMsg &msg)
{
	JyDataWriteStream ds;
	writeHeader(ds, SEC_CMD_ECHO);
	ds.putStr("hi, this is client first send msg.");

	send(sock_, ds);
}

void SimpleEchoClient::onRead(short packetType, unique_ptr<JyDataReadBlock> &ds)
{
	string msg = ds->getStr();

	DBG(("receive server [%s]\n", msg.c_str()));

	JyDataWriteStream dsOut;
	writeHeader(dsOut, SEC_CMD_ECHO);
	dsOut.putStr("this is client send msg.\n");

	send(sock_, dsOut);
}
