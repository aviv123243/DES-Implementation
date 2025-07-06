#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "include/des.h"
#include "include/keyGen.h"
#include "include/modes.h"

#include "inttypes.h"

uint64_t add_padding(uint8_t tailingBytes[],int numOfBytes)
{
    uint8_t bytes[8] = {0};
    uint64_t res;

    memcpy(bytes, tailingBytes, numOfBytes);  

    uint8_t padValue = SIZE_OF_BLOCK_BYTES - numOfBytes;

    for (int i = numOfBytes; i < SIZE_OF_BLOCK_BYTES; i++) {
        bytes[i] = padValue;  
    }

    memcpy(&res, bytes, SIZE_OF_BLOCK_BYTES);

    return res;
}

int get_padding_len(uint64_t block) {
    printf("padded block to remove pad 0x%08" PRIX64 "\n",block);
    uint8_t bytes[8];

    memcpy(bytes,&block,SIZE_OF_BLOCK_BYTES);

    printf("0x%08" PRIX8 "\n",bytes[SIZE_OF_BLOCK_BYTES - 1]);
    return bytes[SIZE_OF_BLOCK_BYTES - 1];  
}

int des_ECB_encrypt_string(const char *str, char *dst, uint64_t key)
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
            block = add_padding(blockBuffer, chunkSize);
            printf("padded block: 0x%016" PRIx64 "\n",block);
        }

        uint64_t encrypted = des_block(block, subKeys, ENCRYPT);
        memcpy(dst + dstIndex, &encrypted, 8);

        dstIndex += 8;
        i += chunkSize;
    }

    return len + SIZE_OF_BLOCK_BYTES;
}

void des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
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
    memcpy(&lastBlock, dst + dstIndex - SIZE_OF_BLOCK_BYTES, SIZE_OF_BLOCK_BYTES);
    int padLen = get_padding_len(lastBlock);

    int actualLength = dstIndex - padLen;
    dst[actualLength] = '\0';
}
