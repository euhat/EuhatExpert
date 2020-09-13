#pragma once

#include <common/JyTcpSelector.h>

enum SimpleFilePacketType
{
	SfPaFileDown = JyTcpCmdUser
};

enum SimpleFileResultType
{
	SfResultOk,
	SfResultFileReachEnd,
	SfResultFileNotExist,
	SfResultFileSeekExceed,
	SfResultFileSeekError
};