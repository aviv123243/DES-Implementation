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

    uint64_t lastBlock;
    memcpy(&lastBlock, dst + dstIndex - SIZE_OF_BLOCK_BYTES, SIZE_OF_BLOCK_BYTES);

    //getting the actual size of the last block (without padding)
    int padLen = get_padding_len(lastBlock);

    //putting a null terminator at the end of the original plaintext
    int actualLength = dstIndex - padLen;
    dst[actualLength] = '\0';
}

void des_ECB_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP) return;

    uint8_t blockBuffer[8];
    size_t bytesRead;

    while ((bytesRead = fread(blockBuffer, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t block;
        memcpy(&block, blockBuffer, SIZE_OF_BLOCK_BYTES);

        uint64_t encrypted = des_block(block, subKeys, ENCRYPT);
        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    // Handle final partial block with padding
    if (bytesRead > 0 || feof(srcP))
    {
        // Pad the final block
        uint64_t lastBlock = add_padding(blockBuffer, bytesRead);
        uint64_t encrypted = des_block(lastBlock, subKeys, ENCRYPT);
        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(srcP);
    fclose(dstP);
}


void des_ECB_decrypt_file(const char *cipher, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *cipherP = fopen(cipher, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!cipherP || !dstP) return;

    uint64_t currentBlock, nextBlock;
    size_t bytesRead;

    // Prime the loop
    bytesRead = fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

    while (bytesRead == SIZE_OF_BLOCK_BYTES)
    {
        bytesRead = fread(&nextBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

        uint64_t decrypted = des_block(currentBlock, subKeys, DECRYPT);

        if (bytesRead < SIZE_OF_BLOCK_BYTES) {
            // This is the last block, strip padding
            int padLen = get_padding_len(decrypted);
            fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES - padLen, dstP);
            
        } else {
            fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
        }

        currentBlock = nextBlock;
    }

    fclose(cipherP);
    fclose(dstP);
}
