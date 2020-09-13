#pragma once

#include <common/JyDataStream.h>
#include <common/JyBigNum.h>

class JyCrc32
{
	uint32_t tab_[256];
public:
	JyCrc32();
	~JyCrc32();

	uint32_t gen(const char *buf, int len, uint32_t c = -1);
};

extern JyCrc32 gJyCrc32;

class JyDataEncrypt
{
public:
	virtual int encrypt(JyDataWriteStream &dsOut, const char *buf, int len) = 0;
};

class JyDataDecrypt
{
public:
	virtual int decrypt(JyDataWriteStream &dsOut, const char *buf, int len) = 0;
};

class JyDataEncryptClear : public JyDataEncrypt
{
public:
	int encrypt(JyDataWriteStream &dsOut, const char *buf, int len);
};

class JyDataDecryptClear : public JyDataDecrypt
{
public:
	int decrypt(JyDataWriteStream &dsOut, const char *buf, int len);
};

class JyDataEncryptAsymmetric : public JyDataEncrypt
{
	JyBigNumCtx ctx_;

public:
	JyDataEncryptAsymmetric();
	~JyDataEncryptAsymmetric();

	int encrypt(JyDataWriteStream &dsOut, const char *buf, int len);

	JyBigNum n_;
	JyBigNum e_;
};

class JyDataDecryptAsymmetric : public JyDataDecrypt
{
	JyBigNumCtx ctx_;

public:
	JyDataDecryptAsymmetric();
	~JyDataDecryptAsymmetric();

	int decrypt(JyDataWriteStream &dsOut, const char *buf, int len);

	JyBigNum n_;
	JyBigNum d_;
};

class JyDataEncryptSymmetric : public JyDataEncrypt
{
public:
	int encrypt(JyDataWriteStream &dsOut, const char *buf, int len);

	JyBuf xor_;
};

class JyDataDecryptSymmetric : public JyDataDecrypt
{
public:
	int decrypt(JyDataWriteStream &dsOut, const char *buf, int len);

	JyBuf xor_;
};