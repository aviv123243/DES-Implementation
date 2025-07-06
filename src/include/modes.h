#include <stdint.h>

uint64_t add_padding(uint64_t blockToPad, int numOfExistingBytes);

char *des_ECB_encrypt_string(const char *str, char *dst, uint64_t key);
char *des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key);