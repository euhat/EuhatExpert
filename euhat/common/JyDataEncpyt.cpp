#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include "JyDataEncrypt.h"
#include <common/JyDataStream.h>
#include <common/WhCommon.h>
#include <EuhatPostDef.h>

#define DEBUG_ENCRYPT_DATA 0

#define CRC32_POLYNOMIAL  0xEDB88320L
#define CRC32_TABLESIZE   256

JyCrc32::JyCrc32()
{
	uint32_t i, j;
	uint32_t crc;

	for (i = 0; i < CRC32_TABLESIZE; i++) {
		crc = i;

		for (j = 0; j < 8; j++) {
			if (crc & 0x00000001L)
				crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
			else
				crc = crc >> 1;
		}

		tab_[i] = crc;
	}
}

JyCrc32::~JyCrc32()
{
}

uint32_t JyCrc32::gen(const char *buf, int len, uint32_t c)
{
	const unsigned char *cp = (unsigned char *)buf;

	/* same with: c = (uint32_t) (~c & 0xFFFFFFFF); */
	c = (uint32_t)(c ^ 0xFFFFFFFF);

	while (len--)
	{
		c = (c >> 8) ^ tab_[(c ^ *cp++) & 0xff];
	}

	return (uint32_t)(~c & 0xFFFFFFFF);
}

JyCrc32 gJyCrc32;

int JyDataEncryptClear::encrypt(JyDataWriteStream &dsOut, const char *buf, int len)
{
	dsOut.putBufWithoutLen(buf, len);
	return 1;
}

int JyDataDecryptClear::decrypt(JyDataWriteStream &dsOut, const char *buf, int len)
{
	dsOut.putBufWithoutLen(buf, len);
	return 1;
}

JyDataEncryptAsymmetric::JyDataEncryptAsymmetric()
	: e_(ctx_),
	  n_(ctx_)
{
}

JyDataEncryptAsymmetric::~JyDataEncryptAsymmetric()
{
	
}

int JyDataEncryptAsymmetric::encrypt(JyDataWriteStream &dsOut, const char *buf, int len)
{
	uint32_t checksum = gJyCrc32.gen(buf, len);
	dsOut.put<uint32_t>(checksum);

	int stepBytes = n_.getBits() / 8 - 1;
	const char *p = buf;
	int leftBytes = len;
	JyBuf stepBuf(stepBytes);
	JyBigNumCtx ctx;
	JyBigNum m(ctx);
	JyBigNum c(ctx);
	while (1)
	{
		int toCopy = 0;
		if (leftBytes >= stepBytes)
		{
			toCopy = stepBytes;
			leftBytes -= stepBytes;
		}
		else
		{
			toCopy = leftBytes;
			leftBytes = 0;
		}
		if (toCopy == 0)
			break;

		int nullCount = 0;
		for (; nullCount < toCopy; nullCount++)
		{
			if (p[nullCount] != 0)
				break;
		}
		dsOut.put((short)nullCount);

		memcpy(stepBuf.data_.get(), p, toCopy);
		p += toCopy;
		stepBuf.size_ = toCopy;

		m.setBuf(stepBuf);
		c.expMod(m, e_, n_);

#if DEBUG_ENCRYPT_DATA
		WhFileGuard g(fopen("encrypt.txt", "a+"));
		fprintf(g.fp_, "\nencrypt, nullCount:%d, toCopy:%d*************************************\n", nullCount, toCopy);
		string hexBuf = JyBigNum::data2hex((unsigned char *)stepBuf.data_.get(), toCopy);
		fprintf(g.fp_, hexBuf.c_str());
		fprintf(g.fp_, "\n================\n");
		fprintf(g.fp_, "\nn______________\n");
		n_.print(g.fp_);
		fprintf(g.fp_, "\nm______________\n");
		m.print(g.fp_);
		fprintf(g.fp_, "\ne______________\n");
		e_.print(g.fp_);
		fprintf(g.fp_, "\nc______________\n");
		c.print(g.fp_);
#endif

		JyBuf bufTmp;
		c.getBuf(bufTmp);
		dsOut.putBuf<short>(bufTmp.data_.get(), bufTmp.size_);
	}
	return 1;
}

JyDataDecryptAsymmetric::JyDataDecryptAsymmetric()
	: d_(ctx_),
	  n_(ctx_)
{
}

JyDataDecryptAsymmetric::~JyDataDecryptAsymmetric()
{
}

int JyDataDecryptAsymmetric::decrypt(JyDataWriteStream &dsOut, const char *buf, int len)
{
	uint32_t checksum = *(uint32_t *)buf;
	buf += sizeof(uint32_t);
	len -= sizeof(uint32_t);

	int idx = dsOut.getIdx();

	JyDataReadStream ds;
	ds.buf_.data_.reset((char *)buf);
	ds.buf_.size_ = len;
	JyBigNumCtx ctx;
	JyBigNum c(ctx);
	JyBigNum m(ctx);
	while (1)
	{
		if (ds.end())
			break;

		short nullCount = ds.get<short>();
#if DEBUG_ENCRYPT_DATA
		short nullCount2 = nullCount;
#endif
		for (; nullCount--; )
			dsOut.put<char>(0);

		JyBuf bufTmp;
		ds.getBuf<short>(bufTmp);
		c.setBuf(bufTmp);

		m.expMod(c, d_, n_);
		
#if DEBUG_ENCRYPT_DATA
		WhFileGuard g(fopen("encrypt.txt", "a+"));
		fprintf(g.fp_, "\ndecrypt, nullCount:%d\n", nullCount2);
		string hexBuf = JyBigNum::data2hex((unsigned char *)bufTmp.data_.get(), bufTmp.size_);
		fprintf(g.fp_, "hex:%s\n", hexBuf.c_str());
		fprintf(g.fp_, "\n================\n");
		fprintf(g.fp_, "\nn______________\n");
		n_.print(g.fp_);
		fprintf(g.fp_, "\nc______________\n");
		c.print(g.fp_);
		fprintf(g.fp_, "\nd______________\n");
		d_.print(g.fp_);
		fprintf(g.fp_, "\nm______________\n");
		m.print(g.fp_);
#endif

		m.getBuf(bufTmp);
		dsOut.putBufWithoutLen(bufTmp.data_.get(), bufTmp.size_);
	}
	ds.buf_.data_.release();

	uint32_t checksumOut = gJyCrc32.gen(dsOut.buf_.data_.get() + idx, dsOut.getIdx() - idx);
	if (checksum != checksumOut)
	{
		DBG(("decrypt checksum is failed.\n"));
		return 0;
	}
	return 1;
}

static int xorData(JyDataWriteStream &dsOut, const char *buf, int len, JyBuf &xorBuf)
{
//	dsOut.putBufWithoutLen(buf, len);
//	return 1;

	int stepBytes = xorBuf.size_;
	const char *p = buf;
	int leftBytes = len;

	dsOut.ensureRoom(len);
	char *pTo = dsOut.allocBufWithoutLen(len);
	while (1)
	{
		int toCopy = 0;
		if (leftBytes >= stepBytes)
		{
			toCopy = stepBytes;
			leftBytes -= stepBytes;
		}
		else
		{
			toCopy = leftBytes;
			leftBytes = 0;
		}
		if (toCopy == 0)
			break;
#if 0
		for (int i = 0; i < toCopy; i++)
		{
			char ch = *p++;
			ch ^= xorBuf.data_.get()[i];
			dsOut.put<char>(ch);
		}
#else
		typedef int64_t unitType;
		char *pFrom = xorBuf.data_.get();
		int i64s = toCopy / (sizeof(unitType) << 2);
		for (; i64s--; )
		{
			*(unitType *)pTo = (*(unitType *)p) ^ (*(unitType *)pFrom);
			p += sizeof(unitType);
			pFrom += sizeof(unitType);
			pTo += sizeof(unitType);

			*(unitType *)pTo = (*(unitType *)p) ^ (*(unitType *)pFrom);
			p += sizeof(unitType);
			pFrom += sizeof(unitType);
			pTo += sizeof(unitType);

			*(unitType *)pTo = (*(unitType *)p) ^ (*(unitType *)pFrom);
			p += sizeof(unitType);
			pFrom += sizeof(unitType);
			pTo += sizeof(unitType);

			*(unitType *)pTo = (*(unitType *)p) ^ (*(unitType *)pFrom);
			p += sizeof(unitType);
			pFrom += sizeof(unitType);
			pTo += sizeof(unitType);
		}
		int i64sMod = toCopy % (sizeof(unitType) * 4);
		for (; i64sMod--; )
		{
			*pTo++ = (*p++) ^ (*pFrom++);
		}
#endif
	}
	return 1;
}

int JyDataEncryptSymmetric::encrypt(JyDataWriteStream &dsOut, const char *buf, int len)
{
	xorData(dsOut, buf, len, xor_);
	return 1;
}

int JyDataDecryptSymmetric::decrypt(JyDataWriteStream &dsOut, const char *buf, int len)
{
	xorData(dsOut, buf, len, xor_);
	return 1;
}
