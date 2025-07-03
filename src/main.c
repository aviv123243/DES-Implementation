#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "include/des.h"
#include "include/keyGen.h"

#include <inttypes.h>

int main()
{
    uint64_t block = 0xAABBCCDD11223344;
    uint64_t key = 0x0123456789ABCDEF;

    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    uint64_t cipherBlock = des_block(block, subKeys, ENCRYPT);

    uint64_t decoded = des_block(cipherBlock, subKeys, DECRYPT);

    printf("Plaintext:  0x%016" PRIx64 "\n", block);
    printf("Ciphertext: 0x%016" PRIx64 "\n", cipherBlock);
    printf("decoded: 0x%016" PRIx64 "\n", decoded);

    return 0;
}
