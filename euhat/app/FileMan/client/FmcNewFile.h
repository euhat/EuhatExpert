#pragma once

#include "FmcTask.h"

class FmcNewFile : public FmcTask
{
public:
	void req(FmTaskNew *task);
	void ack(unique_ptr<JyDataReadBlock> &ds);
};