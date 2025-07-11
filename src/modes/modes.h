#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "../des_block/main_encryption/des.h"
#include "../des_block/subkey_genaration/subKeyGen.h"
#include "../key_iv/key_iv.h"
#include "../pkcs7_padding/pkcs7.h"

#include "inttypes.h"

#define SIZE_OF_BLOCK_BYTES 8

// ECB
int des_ECB_encrypt_string(const char *str, char *dst, uint64_t key);
void des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_ECB_encrypt_file(const char *src, const char *dst, uint64_t key);
void des_ECB_decrypt_file(const char *cipher, const char *dst, uint64_t key);

// CBC
int des_CBC_encrypt_string(const char *str, char *dst, uint64_t key);
void des_CBC_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_CBC_encrypt_file(const char *src, const char *dst, uint64_t key);
void des_CBC_decrypt_file(const char *cipher, const char *dst, uint64_t key);

// PCBC
int des_PCBC_encrypt_string(const char *str, char *dst, uint64_t key);
void des_PCBC_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_PCBC_encrypt_file(const char *src, const char *dst, uint64_t key);
void des_PCBC_decrypt_file(const char *cipher, const char *dst, uint64_t key);

// CFB
int des_CFB_encrypt_string(const char *str, char *dst, uint64_t key);
void des_CFB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_CFB_encrypt_file(const char *src, const char *dst, uint64_t key);
void des_CFB_decrypt_file(const char *cipher, const char *dst, uint64_t key);

// OFB
int des_OFB_encrypt_string(const char *str, char *dst, uint64_t key);
void des_OFB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_OFB_encrypt_file(const char *src, const char *dst, uint64_t key);
void des_OFB_decrypt_file(const char *cipher, const char *dst, uint64_t key);

// CTR
int des_CTR_encrypt_string(const char *str, char *dst, uint64_t key);
void des_CTR_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_CTR_encrypt_file(const char *src, const char *dst, uint64_t key);
void des_CTR_decrypt_file(const char *cipher, const char *dst, uint64_t key);
