#pragma once

#include "FmsTask.h"

class FmsPeerFileList : public FmsTask
{
public:
	void echo(WhSockHandle sock, unique_ptr<JyDataReadBlock> &ds);

};