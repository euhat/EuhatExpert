#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManServer.h"
#include "FmsDownFile.h"
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include <EuhatPostDef.h>

void FmsDownFile::echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	string path = whCorrectFilePath(ds->getStr());
	int64_t filePos = ds->get<int64_t>();
	int reqLen = ds->get<int>();
	if (reqLen > FILE_MAN_RECV_FILE_SEGMENT_MAX_LEN)
		reqLen = FILE_MAN_RECV_FILE_SEGMENT_MAX_LEN;

	JyDataWriteStream dsAck;
	parent_->writeHeader(dsAck, FmCmdFileDown);
	dsAck.put(id);
	dsAck.putStr(path.c_str());

	if (!strIsStartWith(path.c_str(), parent_->baseDir_.c_str()))
	{
		dsAck.put((int)FmResultFileNotExist);
		DBG(("illegal download [%s].\n", path.c_str()));
	}
	else {
	    DBG(("down file:[%s], at pos:%lld, reqLen:%d\n", path.c_str(), filePos, reqLen));
        fmReadFile(dsAck, path.c_str(), filePos, reqLen);
    }

	parent_->send(sock, dsAck);
}