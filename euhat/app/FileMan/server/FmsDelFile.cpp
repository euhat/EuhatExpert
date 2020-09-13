#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManServer.h"
#include "FmsDelFile.h"
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include <EuhatPostDef.h>

void FmsDelFile::echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	string path = ds->getStr();
	int isFolder = ds->get<int>();

	int retResult;
	if (isFolder)
		retResult = opRmDir(path.c_str());
	else
		retResult = opUnlink(path.c_str());

	JyDataWriteStream dsAck;
	parent_->writeHeader(dsAck, FmCmdFileDel);
	dsAck.put(id);
	dsAck.putStr(path.c_str());
	dsAck.put(isFolder);

	dsAck.put(retResult);

	parent_->send(sock, dsAck);
}