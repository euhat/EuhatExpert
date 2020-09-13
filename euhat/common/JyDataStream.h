#pragma once

#include <common/OpCommon.h>
#include <common/JyBuf.h>

class JyDataWriteStream
{
	int idx_;

public:
	JyDataWriteStream();
	~JyDataWriteStream();

	void ensureRoom(int addSize);

	template<class T>
	void put(T t)
	{
		ensureRoom(sizeof(t));
		put(t, idx_);
		idx_ += sizeof(t);
	}

	template<class T>
	void put(T t, int at)
	{
		memcpy(buf_.data_.get() + at, &t, sizeof(t));
	}
	
	template<class T = short>
	void putStr(const char *str)
	{
		putBuf<T>(str, strlen(str) + 1);
	}

	void putBufWithoutLen(const char *buf, int len);

	template<class T = int>
	void putBuf(const char *buf, int len)
	{
		char *ptr = allocBuf<T>(len);
		memcpy(ptr, buf, len);
	}

	template<class T = int>
	char *allocBuf(int bufLen)
	{
		put<T>(bufLen);
		return allocBufWithoutLen(bufLen);
	}

	char *allocBufWithoutLen(int bufLen);

	int size();
	int getIdx() { return idx_; }

	JyBuf buf_;
};

class JyDataWriteBlock : public JyDataWriteStream
{
public:
	JyDataWriteBlock(JyBufPool &pool);
	~JyDataWriteBlock();

	void ensureRoom(int addSize) {}

	JyBufGuard guard_;
};
class JyDataReadStream
{
	int idx_;

public:
	JyDataReadStream();
	~JyDataReadStream();

	template<class T>
	T get()
	{
		T t;
		memcpy(&t, buf_.data_.get() + idx_, sizeof(t));
		idx_ += sizeof(t);
		return t;
	}
	
	template<class T = short>
	char *getStr()
	{
		int len;
		return getBuf<T>(len);
	}

	template<class T = int>
	char *getBuf(int &len)
	{
		len = get<T>();
		char *buf = buf_.data_.get() + idx_;
		idx_ += len;
		return buf;
	}

	template<class T = int>
	void getBuf(JyBuf &buf)
	{
		int len;
		char *p = getBuf<T>(len);
		buf.reset(len);
		memcpy(buf.data_.get(), p, len);
	}

	int getIdx() { return idx_; }
	int end();
	void reset(JyBuf &buf);

	JyBuf buf_;
};

class JyDataReadBlock : public JyDataReadStream
{
	JyBufPool *pool_;
public:
	JyDataReadBlock(JyBufPool &pool, char *base, int len);
	~JyDataReadBlock();
};