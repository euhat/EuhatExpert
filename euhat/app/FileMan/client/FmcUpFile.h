#pragma once

#include "FmcTask.h"

class FmcUpFile : public FmcTask
{
public:
	void req(FmTaskUpload *task);
	void ack(unique_ptr<JyDataReadBlock> &ds);
};