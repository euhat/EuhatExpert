#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
} Md5Ctx;
                                      
void md5Init(Md5Ctx* context);
void md5Update(Md5Ctx* context, unsigned char* input, unsigned int inputlen);
void md5Final(Md5Ctx* context, unsigned char digest[16]);
void md5Transform(unsigned int state[4], unsigned char block[64]);
void md5Encode(unsigned char* output, unsigned int* input, unsigned int len);
void md5Decode(unsigned int* output, unsigned char* input, unsigned int len);

#ifdef __cplusplus
}
#endif