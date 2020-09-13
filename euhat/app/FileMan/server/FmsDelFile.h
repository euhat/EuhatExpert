#pragma once

#include "FmsTask.h"

class FmsDelFile : public FmsTask
{
public:
	void echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);
};