#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include "JyBuf.h"
#include <EuhatPostDef.h>

JyBuf::JyBuf(int size)
{
	reset(size);
}

JyBuf::~JyBuf()
{

}

void JyBuf::reset(JyBuf &in)
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