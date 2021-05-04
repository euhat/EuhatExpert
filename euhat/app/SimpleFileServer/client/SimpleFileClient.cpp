#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "SimpleFileClient.h"
#include <common/JyDataCrypto.h>
#include "../common/SimplePacket.h"
#include "../common/SimpleFileOp.h"
#include <EuhatPostDef.h>

SimpleFileClient::SimpleFileClient(const char *serverIp, int serverPort)
	: JyTcpClient(serverIp, serverPort, 1024 * 1024 * 20, 1)
{

}

SimpleFileClient::~SimpleFileClient()
{
	stop();
}

void SimpleFileClient::onUserStart(JyMsg &msg)
{
	if (!canSend(sock_))
		return;

	opUnlink(localPath_.c_str());

	postMsg(SFC_CMD_RECV_FILE_REQ);
}

void SimpleFileClient::onRecvFileReq(JyMsg &msg)
{
	if (!canSend(sock_))
		return;

	JyDataWriteStream ds;
	writeHeader(ds, SfPaFileDown);
	ds.putStr(peerPath_.c_str());
	ds.put(filePos_);
	ds.put((int)(1024 * 1024 * 1));

	send(sock_, ds);
}

void SimpleFileClient::onRecvFileAck(unique_ptr<JyDataReadBlock> &ds)
{
	string peerPath = ds->getStr();

	int result = ds->get<int>();
	if (result != SfResultOk && result != SfResultFileReachEnd)
	{
		DBG(("recv file failed %d\n", result));
		return;
	}

	int writeSize = sfWriteFile(ds, localPath_.c_str());

	if (result != SfResultFileReachEnd)
	{
		filePos_ += writeSize;
		postMsg(SFC_CMD_RECV_FILE_REQ);
	}
	else
	{
		DBG(("recv file ok.\n"));
	}
}

void SimpleFileClient::onRead(short packetType, unique_ptr<JyDataReadBlock> &ds)
{
	switch (packetType)
	{
	case SfPaFileDown: onRecvFileAck(ds); break;
	default: break;
	}
}

int SimpleFileClient::onWork(JyMsg &msg)
{
	switch (msg.cmd_)
	{
	case SFC_CMD_RECV_FILE_REQ: onRecvFileReq(msg); return 1;
	}
	return JyTcpClient::onWork(msg);
}