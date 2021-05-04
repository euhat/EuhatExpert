#pragma once

#include <common/OpCommon.h>
#include <common/JyMutex.h>

class JyBuf
{
	void reset(JyBuf&& in);
public:
	JyBuf(int size = 0);
	JyBuf(const string& str);
	JyBuf(JyBuf&& in);
	~JyBuf();

	JyBuf& operator=(JyBuf&& in);

	void reset(int len = 0);
	void reset(char *buf, int len);

	static JyBuf md5(const JyBuf& in);
	JyBuf xorData(const JyBuf& other);

	int eq(const JyBuf& other);

	unique_ptr<char []> data_;
	int size_;
};

class JyBufGuard;

class JyBufPool
{
	WhMutex mutex_;
	list<unique_ptr<char[]>> pool_;
	int unitSize_;
	int maxReserved_;

public:
	JyBufPool(int unitSize, int maxReserved);
	~JyBufPool();
	void get(JyBufGuard &guard);
	void ret(char *p);
	int getUnitSize() { return unitSize_; }
};

class JyBufGuard
{
	char *p_;

public:
	JyBufGuard();
	~JyBufGuard();
	void reset(char *p = NULL);
	char *release();
	char *get();

	JyBufPool *pool_;
};