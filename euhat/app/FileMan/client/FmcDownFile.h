#pragma once

#include "FmcTask.h"

class FmcDownFile : public FmcTask
{
public:
	void req(FmTaskDownload *task);
	void ack(unique_ptr<JyDataReadBlock> &ds);
};