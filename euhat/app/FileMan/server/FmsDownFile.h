#pragma once

#include "FmsTask.h"

class FmsDownFile : public FmsTask
{
public:
	void echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);

};