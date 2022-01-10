#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include "JyBigNum.h"
#include <common/JyBuf.h>
#include <openssl/bn.h>
#include <openssl/rand.h>
#include "WhCommon.h"
#include <EuhatPostDef.h>

JyBigNumCtx::JyBigNumCtx()
{
	ctx_ = BN_CTX_new();
}

JyBigNumCtx::~JyBigNumCtx()
{
	BN_CTX_free((BN_CTX *)ctx_);
}

void *JyBigNumCtx::getCtx()
{
	return ctx_;
}

JyBigNum::JyBigNum(JyBigNumCtx &ctx)
{
	ctx_ = &ctx;
	value_ = BN_new();
}

JyBigNum::~JyBigNum()
{
	BN_free((BIGNUM *)value_);
}

void JyBigNum::setValue(unsigned int val)
{
	BN_set_word((BIGNUM *)value_, (BN_ULONG)val);
}

void JyBigNum::setValue(const JyBigNum &bn)
{
	BN_copy((BIGNUM *)value_, (const BIGNUM *)bn.value_);
}

JyBigNum &JyBigNum::mul(unsigned int val)
{
	BN_mul_word((BIGNUM *)value_, val);
	return *this;
}

JyBigNum &JyBigNum::mul(const JyBigNum &bn)
{
	return mul(*this, bn);
}

JyBigNum &JyBigNum::mul(const JyBigNum &l, const JyBigNum &r)
{
	BN_mul((BIGNUM *)value_, (const BIGNUM *)l.value_, (const BIGNUM *)r.value_, (BN_CTX *)ctx_->getCtx());
	return *this;
}

JyBigNum &JyBigNum::sub(unsigned int val)
{
	BN_sub_word((BIGNUM *)value_, val);
	return *this;
}

JyBigNum &JyBigNum::sub(const JyBigNum &bn)
{
	return sub(*this, bn);
}

JyBigNum &JyBigNum::sub(const JyBigNum &l, const JyBigNum &r)
{
	BN_sub((BIGNUM *)value_, (const BIGNUM *)l.value_, (const BIGNUM *)r.value_);
	return *this;
}

JyBigNum &JyBigNum::add(unsigned int val)
{
	BN_add_word((BIGNUM *)value_, val);
	return *this;
}

JyBigNum &JyBigNum::add(const JyBigNum &bn)
{
	return add(*this, bn);
}

JyBigNum &JyBigNum::add(const JyBigNum &l, const JyBigNum &r)
{
	BN_add((BIGNUM *)value_, (const BIGNUM *)l.value_, (const BIGNUM *)r.value_);
	return *this;
}

JyBigNum &JyBigNum::rand(int bits)
{
	// has memory leak.
	//BN_rand((BIGNUM *)value_, bits, -1, 0);
	return *this;
}

void whRandUniformBuf(JyBuf &buf)
{
	char *p = buf.data_.get();
	size_t size = buf.size_;
	for (int i = 0; i < size; i++, p++)
	{
		*p = (char)whRandUniformInt(0, 256);
	}
}

string JyBigNum::data2hex(unsigned char *data, size_t len)
{
	unique_ptr<char[]> str(new char[len * 2 + 1]);
	char *o = str.get();
	char pat[] = "0123456789ABCDEF";
	for (int i = 0; i < len; i++)
	{
		unsigned char ch = data[i];
		*o++ = pat[ch >> 4];
		*o++ = pat[ch & 0xF];
	}
	*o = 0;
	return str.get();
}

JyBuf JyBigNum::hex2data(const char *str)
{
	JyBuf buf(strlen(str) / 2);
	unsigned char *p = (unsigned char *)str;
	char pat[] = "0123456789ABCDEF";
	int idx = 0;
	while (1)
	{
		if (*p == 0)
			break;
		char ch0 = *p++;
		int idx0 = 0;
		while (pat[idx0] != ch0)
			idx0++;
		int idx1 = 0;
		if (*p != 0)
		{
			char ch1 = *p++;
			while (pat[idx1] != ch1)
				idx1++;
		}
		unsigned char byte = (idx0 << 4) + idx1;
		*(unsigned char *)(buf.data_.get() + idx++) = byte;
	}
	return buf;
}

JyBuf JyBigNum::getBuf()
{
	JyBuf buf;
#if 1
	char *p = BN_bn2hex((BIGNUM *)value_);
	if (p[1] == 0)
	{
#if 0
		WhFileGuard g(fopen("encrypt.txt", "a+"));
		fprintf(g.fp_, "\nencounter bn to hex return '0'.\n");
		::MessageBoxA(NULL, "hi,,,,", NULL, 0);
#endif
		DBG(("encounter bn to hex return '0'.\n"));
		buf = hex2data("00");
	}
	else
		buf = hex2data(p);
	OPENSSL_free(p);
#else
	int bits = getBits();
	buf.reset(bits / 8);
	BN_bn2bin((BIGNUM *)value_, (unsigned char *)buf.data_.get());
#endif
	return buf;
}

void JyBigNum::setBuf(JyBuf &buf)
{
#if 1
	string str = data2hex((unsigned char *)buf.data_.get(), buf.size_);
	BIGNUM *a = NULL;
	BN_hex2bn(&a, str.c_str());
	if (NULL != a)
	{
		BN_copy((BIGNUM *)value_, a);
		BN_free(a);
	}
#else
	BN_bin2bn((const unsigned char *)buf.data_.get(), buf.size_, (BIGNUM *)value_);
#endif
}

void JyBigNum::genPrime(int bits)
{
	BN_generate_prime_ex((BIGNUM *)value_, bits, 1, NULL, NULL, NULL);
}

int JyBigNum::getBits()
{
	return BN_num_bits((BIGNUM *)value_);
}

void JyBigNum::getE(JyBigNum &bn, int bits)
{
	unsigned char p256[] =  { 0xC3, 0xF1, 0xDE, 0xC4, 0x3F, 0x8F, 0x1D, 0xE9, 0x7D, 0x04, 0xB1, 0xC7, 0xD7, 0xC5, 0xF8, 0x5D, 0x9F, 0x2D, 0x86, 0x6C, 0xBE, 0xED, 0x54, 0x47, 0x34, 0x85, 0xFC, 0xB9, 0xC0, 0x48, 0xED, 0xA7 };
	unsigned char p512[] =  { 0xC3, 0x37, 0xE0, 0x29, 0x79, 0x47, 0xA5, 0xF6, 0xB2, 0x60, 0x5D, 0x00, 0x48, 0x00, 0x0E, 0x65, 0x47, 0x01, 0x89, 0xDF, 0x7B, 0x91, 0xF8, 0x22, 0x83, 0x51, 0x4B, 0xB2, 0x72, 0x16, 0xDB, 0xF4,
							  0x3E, 0xCD, 0xBF, 0x28, 0xF9, 0xB7, 0xED, 0xF0, 0x06, 0x5B, 0xC0, 0x51, 0xB6, 0x12, 0x92, 0x40, 0x86, 0x94, 0x98, 0xC7, 0xE0, 0xFB, 0xC6, 0x18, 0x3D, 0xE0, 0x70, 0x0F, 0xB2, 0x37, 0x49, 0xEF };
	unsigned char p1024[] = { 0xCC, 0x8B, 0xDA, 0xD5, 0xC4, 0x8A, 0x72, 0x46, 0x51, 0xB7, 0x93, 0xE1, 0x01, 0xF2, 0x12, 0x2F, 0xFF, 0x86, 0xFC, 0x60, 0xAA, 0x15, 0x49, 0xCA, 0xA8, 0xEE, 0x69, 0xCD, 0x0C, 0x3F, 0xA7, 0x81,
							  0x00, 0xC9, 0x91, 0xE6, 0xF2, 0x05, 0x58, 0x40, 0x24, 0x26, 0x12, 0xF9, 0xDB, 0x2A, 0xC1, 0x56, 0x96, 0x72, 0x38, 0x8D, 0xCC, 0x16, 0xA1, 0xCF, 0x90, 0x1C, 0x8D, 0xE5, 0x20, 0x7F, 0x81, 0x00,
							  0x3C, 0x7B, 0xF3, 0xED, 0x19, 0xF6, 0xC5, 0xBC, 0x8C, 0x01, 0x77, 0xC3, 0xB0, 0xD4, 0xCF, 0x05, 0x2E, 0x05, 0x0A, 0x61, 0x0C, 0xE6, 0x70, 0xE3, 0x69, 0x83, 0x79, 0x52, 0xF8, 0xCE, 0x8C, 0x5B,
							  0x2C, 0x8F, 0xDB, 0x46, 0xE0, 0x16, 0x8B, 0xC7, 0x76, 0xE3, 0x05, 0x09, 0x7E, 0x80, 0xA4, 0xED, 0x8B, 0x1D, 0x11, 0xA9, 0xD5, 0x76, 0x8B, 0xAB, 0x6A, 0xC7, 0xFF, 0xD5, 0x11, 0x7F, 0xD9, 0x47 };
	unsigned char p2048[] = { 0xF6, 0xDC, 0x58, 0x30, 0xB5, 0x63, 0x38, 0x82, 0x0A, 0x13, 0xBE, 0x94, 0x66, 0x13, 0x2A, 0x6B, 0x7A, 0x09, 0xFD, 0x2E, 0xC0, 0x4A, 0xE0, 0x0D, 0x78, 0x30, 0x08, 0x09, 0x85, 0x58, 0x64, 0x56,
							  0xDB, 0xC7, 0x4E, 0xC2, 0x00, 0x1D, 0x87, 0x26, 0x0F, 0xBC, 0x29, 0x66, 0x0D, 0xED, 0xB7, 0x1C, 0xA5, 0xE3, 0x32, 0xD1, 0xC5, 0xB6, 0xFC, 0xE5, 0xC9, 0x08, 0x7C, 0x07, 0x1D, 0xDF, 0xD9, 0xDC,
							  0xB8, 0xAE, 0x61, 0x88, 0xBC, 0xDE, 0x4D, 0xC8, 0x4E, 0xD1, 0xD5, 0x4B, 0x6E, 0x9A, 0x45, 0x8E, 0xF1, 0x4E, 0x8C, 0xBB, 0xDD, 0xBC, 0x1A, 0xA7, 0x58, 0xB5, 0x1D, 0xCE, 0xA1, 0x98, 0xB4, 0x38,
							  0xD2, 0xAD, 0x42, 0x62, 0xDB, 0x73, 0x43, 0x85, 0xA4, 0x27, 0x77, 0xBA, 0x98, 0x19, 0xAE, 0xE4, 0xC8, 0xF5, 0x5B, 0x4E, 0xA6, 0x92, 0xA4, 0xB6, 0xEA, 0xCF, 0x5B, 0x26, 0xF6, 0x57, 0xB6, 0x86,
							  0xE3, 0x2C, 0x37, 0xE1, 0x2A, 0xCA, 0x96, 0xB2, 0xF0, 0x52, 0x6B, 0x6F, 0x2F, 0x43, 0xFC, 0x31, 0xD4, 0xE3, 0x4F, 0x6A, 0x06, 0x26, 0xFE, 0x6F, 0x11, 0x7F, 0xCD, 0xBA, 0x03, 0x1E, 0xBB, 0xC1,
							  0x49, 0x63, 0xE2, 0xC5, 0xFE, 0x2D, 0x6C, 0x4B, 0xD1, 0x27, 0xF2, 0x55, 0x21, 0xF9, 0xB8, 0xE7, 0x57, 0x17, 0xA8, 0x3C, 0x16, 0xA7, 0x39, 0x41, 0xA0, 0xEC, 0x6E, 0x85, 0x7B, 0xF5, 0xAD, 0xC1,
							  0xA5, 0xDC, 0x72, 0xA9, 0x7A, 0x78, 0x53, 0x04, 0xE9, 0x51, 0x6D, 0xC4, 0x24, 0xFC, 0x7C, 0xFA, 0x4C, 0xFF, 0x52, 0xFA, 0x87, 0xE9, 0x4B, 0xBD, 0x50, 0x68, 0xFC, 0x21, 0xE2, 0x13, 0x8E, 0x49,
							  0x94, 0x35, 0xC6, 0x95, 0xD5, 0x47, 0x50, 0xE7, 0x11, 0x31, 0x38, 0x09, 0x5E, 0xFB, 0x08, 0xC3, 0xDD, 0xF7, 0x71, 0xF7, 0x76, 0x0D, 0x83, 0xB0, 0xB1, 0x14, 0xF9, 0x8D, 0x91, 0x95, 0x17, 0x47 };

	unsigned char *p = NULL;

	switch (bits)
	{
	case 256: p = p256; break;
	case 512: p = p512; break;
	case 1024: p = p1024; break;
	case 2048: p = p2048; break;
	}
	JyBuf buf;
	buf.reset((char *)p, bits / 8);
	bn.setBuf(buf);
	buf.data_.release();
}

void JyBigNum::print(FILE *fp)
{
	// has memory leak.
	//BN_print_fp(fp, (BIGNUM *)value_);
}

JyBigNum &JyBigNum::modInverse(const JyBigNum &a, const JyBigNum &n)
{
	BN_mod_inverse((BIGNUM *)value_, (const BIGNUM *)a.value_, (const BIGNUM *)n.value_, (BN_CTX *)ctx_->getCtx());
	return *this;
}

JyBigNum &JyBigNum::expMod(const JyBigNum &a, const JyBigNum &power, const JyBigNum &mod)
{
	BN_mod_exp((BIGNUM *)value_, (const BIGNUM *)a.value_, (const BIGNUM *)power.value_, (const BIGNUM *)mod.value_, (BN_CTX *)ctx_->getCtx());
	return *this;
}