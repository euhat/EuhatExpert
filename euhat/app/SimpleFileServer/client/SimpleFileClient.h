#pragma once

#include <common/JyTcpClient.h>
#include <common/JyDataStream.h>

enum SimpleFileClientCmdType
{
	SFC_CMD_RECV_FILE_REQ = JY_MSG_TYPE_USER
};

class SimpleFileClient : public JyTcpClient
{

protected:
	int onWork(JyMsg &msg);
	void onUserStart(JyMsg &msg);
	void onRecvFileReq(JyMsg &msg);
	
	void onRead(short packetType, unique_ptr<JyDataReadBlock> &ds);
	void onRecvFileAck(unique_ptr<JyDataReadBlock> &ds);

public:
	SimpleFileClient(const char *serverIp, int serverPort);
	virtual ~SimpleFileClient();

	int64_t filePos_;
	string peerPath_;
	string localPath_;
};