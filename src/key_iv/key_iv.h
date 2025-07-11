#pragma once
#include <stdint.h>

#define KEY_SIZE_BYTES 8
#define IV_SIZE_BYTES 8
#define NONCE_SIZE_BYTES 4
#define LAST_7_BITS_MASK 0x7f
#define LAST_BYTE_MASK 0xff

uint64_t generate_random_key();
uint64_t generate_random_iv();
uint32_t generate_random_nonce();