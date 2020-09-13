#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManServer.h"
#include <common/JyDataEncrypt.h>
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include <EuhatPostDef.h>

FileManServer::FileManServer(int serverPort)
	: JyTcpServer(serverPort, FILE_MAN_MEM_POOL_UNIT_SIZE, 1)
{
	downFileOp_.parent_ = this;
	upFileOp_.parent_ = this;
	peerFileListOp_.parent_ = this;
	delFileOp_.parent_ = this;
	newFileOp_.parent_ = this;
}

FileManServer::FileManServer()
	: FileManServer(-1)
{

}

FileManServer::~FileManServer()
{
	stop();
}

void FileManServer::onRead(WhSockHandle sock, short packetType, unique_ptr<JyDataReadBlock> &ds)
{
	DBG(("file man server packet type %d.\n", packetType));
	switch (packetType)
	{
	case FmCmdFileDown: downFileOp_.echo(sock, ds); break;
	case FmCmdFileUp: upFileOp_.echo(sock, ds); break;
	case FmCmdGetPeerFileList: peerFileListOp_.echo(sock, ds); break;
	case FmCmdFileDel: delFileOp_.echo(sock, ds); break;
	case FmCmdFileNew: newFileOp_.echo(sock, ds); break;
	}
}

void FileManServer::onWriteHostInfo(JyDataWriteStream &dsAck)
{
	int idx = dsAck.getIdx();
	dsAck.put<int>(0);
	whGetSysInfo(dsAck);
	dsAck.putStr(baseDir_.c_str());
	// TODO: get ui sys info to dsAck here.
	dsAck.put<int>(dsAck.getIdx() - idx - sizeof(int), idx);
}