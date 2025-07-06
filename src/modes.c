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
    uint8_t bytes[8];

    memcpy(bytes,&block,SIZE_OF_BLOCK_BYTES);

    return bytes[SIZE_OF_BLOCK_BYTES - 1];  
}

//encrypting a string in ECB mode
//returning the length of the ciphertext
int des_ECB_encrypt_string(const char *str, char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int len = strlen(str);
    int i = 0;

    uint64_t encrypted;

    int lenOfPrefectBlocks = (len/8)*8;

    while (i < lenOfPrefectBlocks) {
        uint64_t block;
        
        memcpy(&block, str + i, 8);

        encrypted = des_block(block, subKeys, ENCRYPT);

        memcpy(dst + i, &encrypted, SIZE_OF_BLOCK_BYTES);

        i += SIZE_OF_BLOCK_BYTES;
    }

    uint8_t blockBuffer[8] = {0};

    int numOfTailingBytes = len - lenOfPrefectBlocks;

    //load the tailing bytes into a buffer
    memcpy(blockBuffer,str + i,numOfTailingBytes);

    //create the last block (with the padding)
    uint64_t lastBlock = add_padding(blockBuffer,numOfTailingBytes);

    //encrypt te last block
    encrypted = des_block(lastBlock,subKeys,ENCRYPT);

    //add it to the ciphertext
    memcpy(dst + i, &encrypted, SIZE_OF_BLOCK_BYTES);

    //return the length
    return i + SIZE_OF_BLOCK_BYTES;
}

void des_ECB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int dstIndex = 0;

    for (int i = 0; i < length; i += 8) {
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
