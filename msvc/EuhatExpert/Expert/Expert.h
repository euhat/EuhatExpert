#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"
#include "base/EuhatBase.h"

class CExpertApp : public CWinApp
{
public:
	CExpertApp();
	~CExpertApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CExpertApp theApp;
