#include "modes.h"

int des_CBC_encrypt_string(const char *str, char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int len = strlen(str);
    int srcIndex = 0;
    int dstIndex = 0;

    uint64_t encrypted;

    uint64_t prevBlock = generate_random_iv();
    memcpy(dst + dstIndex,&prevBlock ,sizeof(prevBlock));
    dstIndex += IV_SIZE_BYTES;

    int lenOfPrefectBlocks = (len / SIZE_OF_BLOCK_BYTES) * SIZE_OF_BLOCK_BYTES;

    while (srcIndex < lenOfPrefectBlocks)
    {
        uint64_t block;

        memcpy(&block, str + srcIndex, SIZE_OF_BLOCK_BYTES);

        block ^= prevBlock;

        encrypted = des_block(block, subKeys, ENCRYPT);

        prevBlock = encrypted;

        memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

        dstIndex += SIZE_OF_BLOCK_BYTES;
        srcIndex += SIZE_OF_BLOCK_BYTES;
    }

    uint8_t blockBuffer[8] = {0};

    int numOfTailingBytes = len - lenOfPrefectBlocks;

    // load the tailing bytes into a buffer
    memcpy(blockBuffer, str + srcIndex, numOfTailingBytes);

    // create the last block (with the padding)
    uint64_t lastBlock = add_padding(blockBuffer, numOfTailingBytes);

    lastBlock ^= prevBlock;

    // encrypt te last block
    encrypted = des_block(lastBlock, subKeys, ENCRYPT);

    // add it to the ciphertext
    memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

    // return the length
    return dstIndex + SIZE_OF_BLOCK_BYTES;
}

void des_CBC_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
{
    uint64_t subKeys[16];

    generate_sub_keys(key, subKeys);

    int srcIndex = 0;
    int dstIndex = 0;
    
    uint64_t prevBlock;

    memcpy(&prevBlock,cipher + srcIndex,sizeof(prevBlock));
    srcIndex += IV_SIZE_BYTES;

    for (; srcIndex < length; srcIndex += SIZE_OF_BLOCK_BYTES)
    {
        uint64_t block;

        memcpy(&block, cipher + srcIndex, 8);

        uint64_t decrypted = des_block(block, subKeys, DECRYPT);

        decrypted ^= prevBlock;

        prevBlock = block;        

        memcpy(dst + dstIndex, &decrypted, 8);

        dstIndex += SIZE_OF_BLOCK_BYTES;
    }

    uint64_t lastBlock;
    memcpy(&lastBlock, dst + dstIndex - SIZE_OF_BLOCK_BYTES, SIZE_OF_BLOCK_BYTES);

    // getting the actual size of the last block (without padding)
    int padLen = get_padding_len(lastBlock);

    // putting a null terminator at the end of the original plaintext
    int actualLength = dstIndex - padLen; 
    dst[actualLength] = 0;
}

void des_CBC_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP) return;

    uint8_t blockBuffer[8];
    size_t bytesRead;

    uint64_t prevBlock = generate_random_iv();

    //writing the iv
    fwrite(&prevBlock,sizeof(prevBlock),1,dstP);

    while ((bytesRead = fread(blockBuffer, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t block;
        memcpy(&block, blockBuffer, SIZE_OF_BLOCK_BYTES);

        //xor-ing with the previus block (or iv for the first round)
        //as instructed for the CBC mode
        block ^= prevBlock;

        uint64_t encrypted = des_block(block, subKeys, ENCRYPT);

        prevBlock = encrypted;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    // Pad the final block
    uint64_t lastBlock = add_padding(blockBuffer, bytesRead);

    //xor it with prev
    lastBlock ^= prevBlock;

    uint64_t encrypted = des_block(lastBlock, subKeys, ENCRYPT);
    fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    

    fclose(srcP);
    fclose(dstP);
}

void des_CBC_decrypt_file(const char *cipher, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *cipherP = fopen(cipher, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!cipherP || !dstP)
        return;

    uint64_t currentBlock, nextBlock;
    uint64_t prevBlock;
    size_t bytesRead;

    //reading the iv
    fread(&prevBlock, sizeof(uint64_t), 1, cipherP);
    
    bytesRead = fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

    while (bytesRead == SIZE_OF_BLOCK_BYTES)
    {
        bytesRead = fread(&nextBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

        uint64_t decrypted = des_block(currentBlock, subKeys, DECRYPT);

        decrypted ^= prevBlock;

        prevBlock = currentBlock;

        int isLast = (bytesRead == 0);

        if (isLast)
        {
            // This is the last block, strip padding
            int padLen = get_padding_len(decrypted);
            fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES - padLen, dstP);
        }
        else
        {
            fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
        }

        currentBlock = nextBlock;
    }

    fclose(cipherP);
    fclose(dstP);
}