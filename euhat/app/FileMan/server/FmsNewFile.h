#pragma once

#include "FmsTask.h"

class FmsNewFile : public FmsTask
{
public:
	void echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);
};