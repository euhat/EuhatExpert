#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/JyDataStream.h>
#include <common/md5.h>
#include <common/JyDataCrypto.h>
#include "JyBuf.h"
#include <EuhatPostDef.h>

JyBuf::JyBuf(int len)
{
	reset(len);
}

JyBuf::JyBuf(const string& str)
{
	reset(opMemDup(str.c_str(), str.length()), str.length());
}

JyBuf::JyBuf(JyBuf&& in)
{
	reset(std::move(in));
}

JyBuf::~JyBuf()
{

}

JyBuf& JyBuf::operator=(JyBuf&& in)
{
	reset(std::move(in));
	return *this;
}

void JyBuf::reset(JyBuf&& in)
{
	data_.reset(in.data_.release());
	size_ = in.size_;
	in.size_ = 0;
}

void JyBuf::reset(int len)
{
	if (len > 0)
		data_.reset(new char[len]);
	else
		data_.reset();
	size_ = len;
}

void JyBuf::reset(char *buf, int len)
{
	data_.reset(buf);
	size_ = len;
}

JyBuf JyBuf::md5(const JyBuf& in)
{
	JyBuf out(16);
	Md5Ctx md5;
	md5Init(&md5);
	md5Update(&md5, (unsigned char *)in.data_.get(), in.size_);
	md5Final(&md5, (unsigned char *)out.data_.get());
	return std::move(out);
}

JyBuf JyBuf::xorData(const JyBuf& other)
{
	JyDataWriteStream ds;
	::xorData(ds, data_.get(), size_, other);
	JyBuf buf = std::move(ds.buf_);
	buf.size_ = ds.size();
	return std::move(buf);
}

int JyBuf::eq(const JyBuf& other)
{
	return size_ == other.size_ && opMemEq(data_.get(), other.data_.get(), size_);
}

JyBufPool::JyBufPool(int unitSize, int maxReserved)
{
	unitSize_ = unitSize;
	maxReserved_ = maxReserved;
}

JyBufPool::~JyBufPool()
{

}

void JyBufPool::get(JyBufGuard &guard)
{
	WhMutexGuard g(&mutex_);
	guard.pool_ = this;
	if (pool_.size() == 0)
	{
		guard.reset(new char[unitSize_]);
		return;
	}
	guard.reset(pool_.front().release());
	pool_.pop_front();
}

void JyBufPool::ret(char *p)
{
	WhMutexGuard guard(&mutex_);
	unique_ptr<char[]> ptr(p);
	pool_.push_back(move(ptr));

	if (pool_.size() > (size_t)maxReserved_)
		pool_.pop_front();
}

JyBufGuard::JyBufGuard()
{
	pool_ = NULL;
	p_ = NULL;
}

JyBufGuard::~JyBufGuard()
{
	reset();
}

void JyBufGuard::reset(char *p)
{
	if (NULL != p_)
	{
		pool_->ret(p_);
	}
	p_ = p;
}

char *JyBufGuard::release()
{
	char *p = p_;
	p_ = NULL;
	return p;
}

char *JyBufGuard::get()
{
	return p_;
}