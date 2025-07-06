#include <stdint.h>
#include "../pkcs7_padding/pkcs7.h"

#define SIZE_OF_BLOCK_BYTES 8

int des_ECB_encrypt_string(const char *str, char *dst, uint64_t key);
void des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);

void des_ECB_encrypt_file(const char *src, const char * dst, uint64_t key);
void des_ECB_decrypt_file(const char *cipher, const char * dst, uint64_t key);
