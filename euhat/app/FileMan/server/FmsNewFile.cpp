#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManServer.h"
#include "FmsNewFile.h"
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include <EuhatPostDef.h>

void FmsNewFile::echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	string path = ds->getStr();
	int isFolder = ds->get<int>();

	int retResult = 0;
	if (isFolder)
	{
		whPrepareFolder(path.c_str());
		retResult = opMkDir(path.c_str());
	}
	else
	{
		WhFileGuard g(whFopen(path.c_str(), "wb+"));
		retResult = (g.fp_ != NULL);
	}

	JyDataWriteStream dsAck;
	parent_->writeHeader(dsAck, FmCmdFileNew);
	dsAck.put(id);
	dsAck.putStr(path.c_str());
	dsAck.put(isFolder);

	dsAck.put(retResult);

	parent_->send(sock, dsAck);
}