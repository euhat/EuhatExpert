#pragma once

#include "FmcTask.h"

class FmcPeerFileList : public FmcTask
{
public:
	void req(FmTaskGetSubList *task);
	void ack(unique_ptr<JyDataReadBlock> &ds);
};