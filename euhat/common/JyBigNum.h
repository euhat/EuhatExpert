#pragma once

class JyBigNumCtx
{
	void *ctx_;

public:
	JyBigNumCtx();
	~JyBigNumCtx();
	void *getCtx();
};

class JyBuf;

class JyBigNum
{
	JyBigNumCtx *ctx_;
	void *value_;

public:
	JyBigNum(JyBigNumCtx &ctx);
	~JyBigNum();

	void setValue(unsigned int val);
	void setValue(const JyBigNum &bn);
	JyBigNum &mul(unsigned int val);
	JyBigNum &mul(const JyBigNum &bn);
	JyBigNum &mul(const JyBigNum &l, const JyBigNum &r);
	JyBigNum &sub(unsigned int val);
	JyBigNum &sub(const JyBigNum &bn);
	JyBigNum &sub(const JyBigNum &l, const JyBigNum &r);
	JyBigNum &add(unsigned int val);
	JyBigNum &add(const JyBigNum &bn);
	JyBigNum &add(const JyBigNum &l, const JyBigNum &r);
	JyBigNum &rand(int bits);

	int getBits();

	void genPrime(int bits);
	
	static string data2hex(unsigned char *data, int len);
	static JyBuf hex2data(const char *str);
	static void getE(JyBigNum &bn, int bits);

	JyBigNum &modInverse(const JyBigNum &a, const JyBigNum &n);
	JyBigNum &expMod(const JyBigNum &a, const JyBigNum &power, const JyBigNum &mod);
	void print(FILE *fp = stdout);

	JyBuf getBuf();
	void setBuf(JyBuf &buf);
};

void whRandUniformBuf(JyBuf &buf);