/*
 * Copyright 1995-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <openssl/crypto.h>

void *CRYPTO_malloc(size_t num, const char *file, int line)
{
    return malloc(num);
}

void *CRYPTO_zalloc(size_t num, const char *file, int line)
{
    void *ret;

    ret = CRYPTO_malloc(num, file, line);
    if (ret != NULL)
        memset(ret, 0, num);

    return ret;
}

void *CRYPTO_realloc(void *str, size_t num, const char *file, int line)
{
    return realloc(str, num);
}

void *CRYPTO_clear_realloc(void *str, size_t old_len, size_t num,
                           const char *file, int line)
{
    void *ret = NULL;

    if (str == NULL)
        return CRYPTO_malloc(num, file, line);

    if (num == 0) {
        CRYPTO_clear_free(str, old_len, file, line);
        return NULL;
    }

    /* Can't shrink the buffer since memcpy below copies |old_len| bytes. */
    if (num < old_len) {
        OPENSSL_cleanse((char*)str + num, old_len - num);
        return str;
    }

    ret = CRYPTO_malloc(num, file, line);
    if (ret != NULL) {
        memcpy(ret, str, old_len);
        CRYPTO_clear_free(str, old_len, file, line);
    }
    return ret;
}

void CRYPTO_free(void *str, const char *file, int line)
{
    free(str);
}

void CRYPTO_clear_free(void *str, size_t num, const char *file, int line)
{
    if (str == NULL)
        return;
    if (num)
        OPENSSL_cleanse(str, num);
    CRYPTO_free(str, file, line);
}

void OPENSSL_cleanse(void* str, size_t num)
{
    memset(str, 0, num);
}

char* CRYPTO_strdup(const char* str, const char* file, int line)
{
    char* ret;

    if (str == NULL)
        return NULL;
    ret = CRYPTO_malloc(strlen(str) + 1, file, line);
    if (ret != NULL)
        strcpy(ret, str);
    return ret;
}

int OPENSSL_hexchar2int(unsigned char c)
{
#ifdef CHARSET_EBCDIC
    c = os_toebcdic[c];
#endif

    switch (c) {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'a': case 'A':
        return 0x0A;
    case 'b': case 'B':
        return 0x0B;
    case 'c': case 'C':
        return 0x0C;
    case 'd': case 'D':
        return 0x0D;
    case 'e': case 'E':
        return 0x0E;
    case 'f': case 'F':
        return 0x0F;
    }
    return -1;
}