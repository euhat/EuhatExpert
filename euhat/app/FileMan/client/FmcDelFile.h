#pragma once

#include "FmcTask.h"

class FmcDelFile : public FmcTask
{
public:
	void req(FmTaskDel *task);
	void ack(unique_ptr<JyDataReadBlock> &ds);
};