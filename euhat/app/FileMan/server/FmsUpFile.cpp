#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "FileManServer.h"
#include "FmsUpFile.h"
#include "../common/FileManType.h"
#include "../common/FileManFileOp.h"
#include <EuhatPostDef.h>

void FmsUpFile::echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds)
{
	int id = ds->get<int>();
	string path = ds->getStr();
	time_t lastWriteTime = ds->get<int64_t>();
	int64_t totalFileSize = ds->get<int64_t>();
	int64_t filePos = ds->get<int64_t>();

	int retResult = FmResultOk;
	if (!strIsStartWith(path.c_str(), parent_->baseDir_.c_str()))
	{
		retResult = FmResultNoSpace;
		DBG(("illegal upload [%s].\n", path.c_str()));
	}
	else
	{
		if (filePos < 0)
		{
			int64_t totalSpaceInMB = 0;
			int64_t freeSpaceInMB = 0;
			whGetDiskUsage(path.c_str(), totalSpaceInMB, freeSpaceInMB);
			if (freeSpaceInMB - totalFileSize / (1024 * 1024) < 100)
			{
				retResult = FmResultNoSpace;
				DBG(("server lack free space to write [%s], need space %lld.\n", path.c_str(), totalFileSize));
			}
			else
			{
				whPrepareFolder(path.c_str());
				opUnlink(path.c_str());
				filePos = 0;
			}
		}
		else
		{
			retResult = ds->get<int>();

			int writeSize = 0;
			if (retResult == FmResultOk || retResult == FmResultFileReachEnd)
			{
				writeSize = fmWriteFile(ds, path.c_str());
				if (writeSize < 0)
					retResult = FmResultFailed;
				else
					filePos += writeSize;
			}
		}
	}

	JyDataWriteStream dsAck;
	parent_->writeHeader(dsAck, FmCmdFileUp);
	dsAck.put(id);
	dsAck.putStr(path.c_str());
	dsAck.put(filePos);

	if (retResult == FmResultFileReachEnd)
	{
		whSetFileLastWriteTime(path.c_str(), lastWriteTime);
		DBG(("file [%s] upload ok.\n", path.c_str()));
	}

	dsAck.put(retResult);

	parent_->send(sock, dsAck);
}