#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include "JyDataStream.h"
#include <EuhatPostDef.h>

#define JY_ENCRYPT_DATA_ROOM_UNIT_SIZE (1024 * 8)

JyDataWriteStream::JyDataWriteStream()
{
	idx_ = 0;
}

JyDataWriteStream::~JyDataWriteStream()
{

}

void JyDataWriteStream::ensureRoom(size_t addSize)
{
	if (idx_ + addSize > buf_.size_)
	{
		size_t dataSize = buf_.size_;
		if (dataSize == 0)
			dataSize = JY_ENCRYPT_DATA_ROOM_UNIT_SIZE;
		while (idx_ + addSize > dataSize)
			dataSize *= 2;
		unique_ptr<char[]> data(new char[dataSize]);
		if (buf_.size_ > 0)
			memcpy(data.get(), buf_.data_.get(), buf_.size_);
		buf_.data_.reset(data.release());
		buf_.size_ = dataSize;
	}
}

void JyDataWriteStream::putBufWithoutLen(const char *buf, size_t len)
{
	ensureRoom(len);
	char *ptr = buf_.data_.get() + idx_;
	memcpy(ptr, buf, len);
	idx_ += (int)len;
}

int JyDataWriteStream::size()
{
	return idx_;
}

char *JyDataWriteStream::allocBufWithoutLen(size_t bufLen)
{
	ensureRoom(bufLen);
	char *ptr = buf_.data_.get() + idx_;
	idx_ += (int)bufLen;
	return ptr;
}

JyDataWriteBlock::JyDataWriteBlock(JyBufPool &pool)
	: JyDataWriteStream()
{
	pool.get(guard_);
	buf_.data_.reset(guard_.get());
	buf_.size_ = pool.getUnitSize();
}

JyDataWriteBlock::~JyDataWriteBlock()
{
	buf_.data_.release();
}

JyDataReadStream::JyDataReadStream()
{
	idx_ = 0;
}

JyDataReadStream::~JyDataReadStream()
{

}

int JyDataReadStream::end()
{
	return idx_ >= buf_.size_;
}

void JyDataReadStream::reset(JyBuf &buf)
{
	buf_.data_.reset(buf.data_.release());
	buf_.size_ = buf.size_;
	idx_ = 0;
}

JyDataReadBlock::JyDataReadBlock(JyBufPool &pool, char *base, int len)
	: JyDataReadStream()
{
	pool_ = &pool;
	buf_.data_.reset(base);
	buf_.size_ = len;
}

JyDataReadBlock::~JyDataReadBlock()
{
	char *base = buf_.data_.release();
	pool_->ret(base);
}