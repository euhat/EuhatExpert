#pragma once

#include <common/JyTcpSelector.h>

enum FileManCmdType
{
	FmCmdNone = JyTcpCmdUser,
	FmCmdGetPeerFileList,
	FmCmdFileDown,
	FmCmdFileUp,
	FmCmdFileNew,
	FmCmdFileDel
};

enum FileManResultType
{
	FmResultOk,
	FmResultFailed,
	FmResultVisitCodeMismatch,
	FmResultNoSpace,
	FmResultFileReachEnd,
	FmResultFileNotExist,
	FmResultFileSeekExceed,
	FmResultFileSeekError,

	FmResultGetFileSize
};

#define FILE_MAN_MEM_POOL_UNIT_SIZE			(1024 * 1024 * 20)
#define FILE_MAN_RECV_FILE_SEGMENT_MAX_LEN	((int)(1024 * 1024 * 19.9))
#define FILE_MAN_SEND_FILE_SEGMENT_MAX_LEN	((int)(1024 * 1024 * 19.9))

#define FILE_MAN_DEFAULT_PORT 8083