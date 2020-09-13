#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/WhCommon.h>
#include "SimpleFileOp.h"
#include "SimplePacket.h"
#include <EuhatPostDef.h>

void sfReadFile(JyDataWriteStream &ds, const char *path, int64_t pos, int len)
{
	ds.putStr(path);

	int result = SfResultOk;

	WhFileGuard g(fopen(path, "rb"));
	if (NULL == g.fp_)
	{
		ds.put((int)SfResultFileNotExist);
		return;
	}

	whFseek64(g.fp_, 0, SEEK_END);
	int64_t endPos = whFtell64(g.fp_);
	if (endPos <= pos)
	{
		ds.put((int)SfResultFileSeekExceed);
		return;
	}

	whFseek64(g.fp_, pos, SEEK_SET);
	int64_t truePos = whFtell64(g.fp_);

	if (truePos != pos)
	{
		ds.put((int)SfResultFileSeekError);
		return;
	}

	int64_t trueLeft = endPos - truePos;
	if (trueLeft > (int64_t)len)
		trueLeft = len;
	else
		result = SfResultFileReachEnd;

	int toReadLen = (int)trueLeft;

	ds.put(result);
	ds.put(pos);
	char *ptr = ds.allocBuf(toReadLen);
	fread(ptr, 1, toReadLen, g.fp_);

	return;
}

int sfWriteFile(unique_ptr<JyDataReadBlock> &ds, const char *path)
{
	WhFileGuard g(fopen(path, "ab+"));
	int64_t pos = ds->get<int64_t>();
	int len;
	char *buf = ds->getBuf(len);
	fwrite(buf, 1, len, g.fp_);

	return len;
}