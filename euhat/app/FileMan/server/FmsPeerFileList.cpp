#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManServer.h"
#include "FmsPeerFileList.h"
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include <EuhatPostDef.h>

void FmsPeerFileList::echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	string path = whCorrectFilePath(ds->getStr());

	JyDataWriteStream dsAck;
	parent_->writeHeader(dsAck, FmCmdGetPeerFileList);
	dsAck.put(id);
	dsAck.putStr(path.c_str());
	int idx = dsAck.getIdx();
	dsAck.put<int>(0);
	dsAck.put<int>(0);
	int isOk = 1;
	if (!strIsStartWith(path.c_str(), parent_->baseDir_.c_str()))
		isOk = 0;
	else
		isOk = whGetSubFiles(path.c_str(), dsAck);
	dsAck.put<int>(isOk, idx);
	idx += sizeof(int);
	dsAck.put<int>(dsAck.getIdx() - idx - sizeof(int), idx);

	parent_->send(sock, dsAck);
}