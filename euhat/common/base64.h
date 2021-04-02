#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int base64EncodeLen(int len);
int base64Encode(char *coded_dst, const char *plain_src, int len_plain_src);

int base64DecodeLen(const char *coded_src);
int Base64Decode(char *plain_dst, const char *coded_src);

#ifdef __cplusplus
}
#endif
