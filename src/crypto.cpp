//------------------------------------------------------------------------------
/*
    This file is part of Radar Wallet: http://www.radarlab.org
    Copyright (c) 2015 - 2018 Radar Laboratory

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include "crypto.h"
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string.h>

std::string Crypto::DES_Encrypt(const std::string &key, const std::string &src)
{
    if (src.empty()) {
        return "";
    }
    DES_cblock key_encrypt;
    memset(key_encrypt, 0, 8);
    if (key.size() <= 8) {
        memcpy(key_encrypt, key.data(), key.size());
    } else {
        memcpy(key_encrypt, key.data(), 8);
    }
    DES_key_schedule key_schedule;
    DES_set_key_unchecked(&key_encrypt, &key_schedule);

    const_DES_cblock input_text;
    DES_cblock output_text;
    std::string cipher_text;

    for (size_t i = 0; i < src.size() / 8; i++) {
        memcpy(input_text, src.data() + i * 8, 8);
        DES_ecb_encrypt(&input_text, &output_text, &key_schedule, DES_ENCRYPT);
        for (int j = 0; j < 8; j++) {
            cipher_text.push_back(output_text[j]);
        }
    }

    if (src.size() % 8 != 0) {
        int tmp1 = (int)src.size() / 8 * 8;
        int tmp2 = (int)src.size() - tmp1;
        memset(input_text, 0, 8);
        memcpy(input_text, src.data() + tmp1, tmp2);
        DES_ecb_encrypt(&input_text, &output_text, &key_schedule, DES_ENCRYPT);
        for (int j = 0; j < 8; j++) {
            cipher_text.push_back(output_text[j]);
        }
    }

    return cipher_text;
}

std::string Crypto::DES_Decrypt(const std::string &key, const std::string &src)
{
    if (src.empty()) {
        return "";
    }
    DES_cblock key_encrypt;
    memset(key_encrypt, 0, 8);
    if (key.size() <= 8) {
        memcpy(key_encrypt, key.data(), key.size());
    } else {
        memcpy(key_encrypt, key.data(), 8);
    }
    DES_key_schedule key_schedule;
    DES_set_key_unchecked(&key_encrypt, &key_schedule);

    const_DES_cblock input_text;
    DES_cblock output_text;
    std::string clear_text;

    for (size_t i = 0; i < src.size() / 8; i++) {
        memcpy(input_text, src.data() + i * 8, 8);
        DES_ecb_encrypt(&input_text, &output_text, &key_schedule, DES_DECRYPT);
        for (int j = 0; j < 8; j++) {
            clear_text.push_back(output_text[j]);
        }
    }

    if (src.size() % 8 != 0) {
        int tmp1 = (int)src.size() / 8 * 8;
        int tmp2 = (int)src.size() - tmp1;
        memset(input_text, 0, 8);
        memcpy(input_text, src.data() + tmp1, tmp2);
        DES_ecb_encrypt(&input_text, &output_text, &key_schedule, DES_DECRYPT);
        for (int j = 0; j < 8; j++) {
            clear_text.push_back(output_text[j]);
        }
    }
    return clear_text;
}

std::string Crypto::Base64_Encrypt(const std::string &src)
{
    if (src.empty()) {
        return "";
    }
    BIO * b64 = BIO_new(BIO_f_base64());
    BIO * bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, src.data(), (int)src.size());
    BIO_flush(b64);
    BUF_MEM * bptr = NULL;
    BIO_get_mem_ptr(b64, &bptr);
    std::string res = std::string(bptr->data, bptr->length);
    BIO_free_all(b64);
    return res;
}

std::string Crypto::Base64_Decrypt(const std::string &src)
{
    if (src.empty()) {
        return "";
    }
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bmem = BIO_new_mem_buf(src.data(), (int)src.size());
    bmem = BIO_push(b64, bmem);
    char *buf = new char[src.size() + 1];
    memset(buf, 0, src.size() + 1);
    BIO_read(bmem, buf, (int)src.size());
    BIO_free_all(bmem);
    std::string res = buf;
    delete [] buf;
    return res;
}
