#include <stdint.h>

#define SIZE_OF_BLOCK_BYTES 8

uint64_t add_padding(uint8_t tailingBytes[],int numOfBytes);
int get_padding_len(uint64_t block);

int des_ECB_encrypt_string(const char *str, char *dst, uint64_t key);
void des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);