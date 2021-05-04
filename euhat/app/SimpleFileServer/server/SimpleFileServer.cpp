#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "SimpleFileServer.h"
#include <common/JyDataCrypto.h>
#include "../common/SimplePacket.h"
#include "../common/SimpleFileOp.h"
#include <EuhatPostDef.h>

SimpleFileServer::SimpleFileServer(int serverPort)
	: JyTcpServer(serverPort, 1024 * 1024 * 20, 1)
{

}

SimpleFileServer::~SimpleFileServer()
{
	stop();
}

#define FILE_MAN_SEND_FILE_SEGMENT_MAX_LEN (1024 * 1024 * 10)

void SimpleFileServer::onRecvFileReq(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	string path = ds->getStr();
	int64_t filePos = ds->get<int64_t>();
	int reqLen = ds->get<int>();
	if (reqLen > FILE_MAN_SEND_FILE_SEGMENT_MAX_LEN)
		reqLen = FILE_MAN_SEND_FILE_SEGMENT_MAX_LEN;

	JyDataWriteStream dsAck;
	writeHeader(dsAck, SfPaFileDown);
	sfReadFile(dsAck, path.c_str(), filePos, reqLen);

	send(sock, dsAck);
}

void SimpleFileServer::onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds)
{
	switch (packetType)
	{
	case SfPaFileDown: onRecvFileReq(sock, ds); break;
	default: break;
	}
}

int SimpleFileServer::onWork(JyMsg &msg)
{
/*	switch (msg.cmd_)
	{
	default: 
	}
*/	return JyTcpServer::onWork(msg);
}