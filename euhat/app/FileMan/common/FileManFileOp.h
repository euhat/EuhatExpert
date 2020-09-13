#pragma once

#include <common/JyDataStream.h>
#include <common/JyBuf.h>
#include <dbop/DbOpIni.h>

class FmScheduler;
class JyMsgLoop;
class FmTaskGetSubList;

void fmReadFile(JyDataWriteStream &ds, const char *path, int64_t pos, int len);
int fmWriteFile(unique_ptr<JyDataReadBlock> &ds, const char *path);

void fmGenAndWriteCert(DbOpIni &ini, const char *keyName, int bits);

void dispatchSubList(FmScheduler *scheduler, JyMsgLoop *loop, FmTaskGetSubList &task, JyDataReadStream &ds);