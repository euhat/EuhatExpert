#pragma once

#ifdef __cplusplus
extern "C" {
#endif

size_t base64EncodeLen(size_t len);
size_t base64Encode(char *coded_dst, const char *plain_src, size_t len_plain_src);

size_t base64DecodeLen(const char *coded_src);
size_t Base64Decode(char *plain_dst, const char *coded_src);

#ifdef __cplusplus
}
#endif
