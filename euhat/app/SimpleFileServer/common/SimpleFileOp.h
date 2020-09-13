#pragma once

#include <common/JyDataStream.h>

void sfReadFile(JyDataWriteStream &ds, const char *path, int64_t pos, int len);
int sfWriteFile(unique_ptr<JyDataReadBlock> &ds, const char *path);