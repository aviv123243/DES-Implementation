#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "key_iv.h"

uint64_t generate_random_64();

uint64_t generate_random_key()
{
    srand(time(NULL) ^ clock());

    uint64_t key = 0;
    uint8_t currByte;

    for (int i = 0; i < KEY_SIZE_BYTES; i++)
    {
        // first 7 random bits
        currByte = rand() & LAST_7_BITS_MASK;

        // calculating the parity
        uint8_t parity = __builtin_parity(currByte) ? 0 : 1;

        // appending the parity
        currByte = (currByte << 1) | parity;

        // appending the byte
        key |= ((uint64_t)currByte << i * 8);
    }

    return key;
}

uint64_t generate_random_iv()
{
    return generate_random_64();
}

uint64_t generate_random_nonce()
{
    return generate_random_64();
}

uint64_t generate_random_64()
{
    srand(time(NULL) ^ clock());

    uint64_t iv = 0;

    for (int i = 0; i < IV_SIZE_BYTES; i++)
    {
        //genarating the next byte
        uint8_t randByte = rand() & LAST_BYTE_MASK;

        //appending it to the iv
        iv |= ((uint64_t)randByte << i*8);
    }

    return iv;
}
