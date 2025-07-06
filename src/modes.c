#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/des.h"
#include "include/keyGen.h"

#include <inttypes.h>

#define SIZE_OF_BLOCK_BYTES 8

uint64_t add_padding(uint64_t blockToPad, int numOfExistingBytes)
{
    uint8_t bytes[8];
    memcpy(bytes, &blockToPad, 8);  

    uint8_t padValue = SIZE_OF_BLOCK_BYTES - numOfExistingBytes;

    for (int i = 0; i < padValue; i++) {
        bytes[i] = padValue;  
    }

    uint64_t result;
    memcpy(&result, bytes, 8);
    return result;
}



int get_padding_length(uint64_t paddedBlock)
{
    uint64_t lastByteMask = 0xff;

    return paddedBlock & lastByteMask;
}

char *des_ECB_encrypt_string(const char *str, char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int len = strlen(str);
    int i = 0;
    int dstIndex = 0;

    while (i < len) {
        uint8_t blockBuffer[8] = {0};
        int remaining = len - i;
        int chunkSize = (remaining >= 8) ? 8 : remaining;

        memcpy(blockBuffer, str + i, chunkSize);

        uint64_t block;
        memcpy(&block, blockBuffer, 8);

        if (chunkSize < 8) {
            block = add_padding(block, chunkSize);
            printf("padded block: 0x%016" PRIx64 "\n",block);
        }

        uint64_t encrypted = des_block(block, subKeys, ENCRYPT);
        memcpy(dst + dstIndex, &encrypted, 8);

        dstIndex += 8;
        i += chunkSize;
    }

    return dst;
}


char *des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int i;
    int dstIndex = 0;

    for (i = 0; i < length; i += 8) {
        uint64_t block;
        memcpy(&block, cipher + i, 8);

        uint64_t decrypted = des_block(block, subKeys, DECRYPT);
        memcpy(dst + dstIndex, &decrypted, 8);

        dstIndex += 8;
    }

    // Remove padding length from final block
    uint64_t lastBlock;
    memcpy(&lastBlock, dst + dstIndex - 8, 8);
    int padLen = get_padding_length(lastBlock);

    memset(dst + dstIndex - 8, 0, SIZE_OF_BLOCK_BYTES-padLen);

    // Null-terminate after stripping padding
    int actualLength = dstIndex - padLen;
    dst[actualLength] = '\0';

    return dst;
}
