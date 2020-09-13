#pragma once

#include "FmsTask.h"

class FmsUpFile : public FmsTask
{
public:
	void echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);
};