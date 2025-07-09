#include "modes.h"

int des_OFB_encrypt_string(const char *str, char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int len = strlen(str);
    int srcIndex = 0;
    int dstIndex = 0;

    uint64_t encrypted;
    uint64_t encryptedPrev;

    uint64_t prevBlock = generate_random_iv();
    memcpy(dst + dstIndex,&prevBlock ,sizeof(prevBlock));
    dstIndex += IV_SIZE_BYTES;

    while (srcIndex < len)
    {
        encryptedPrev = des_block(prevBlock,subKeys,ENCRYPT);

        uint64_t block;

        memcpy(&block, str + srcIndex, SIZE_OF_BLOCK_BYTES);

        encrypted = encryptedPrev ^block;
        prevBlock = encryptedPrev;

        memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

        dstIndex += SIZE_OF_BLOCK_BYTES;
        srcIndex += SIZE_OF_BLOCK_BYTES;
    }

    // add it to the ciphertext
    memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

    // return the length        
    return dstIndex + SIZE_OF_BLOCK_BYTES;
}

void des_OFB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
{
    uint64_t subKeys[16];

    generate_sub_keys(key, subKeys);

    int srcIndex = 0;
    int dstIndex = 0;
    
    uint64_t prevBlock;
    uint64_t encryptedPrev;
    uint64_t decrypted;

    memcpy(&prevBlock,cipher + srcIndex,sizeof(prevBlock));
    srcIndex += IV_SIZE_BYTES;

    for (; srcIndex < length; srcIndex += SIZE_OF_BLOCK_BYTES)
    {
        encryptedPrev = des_block(prevBlock,subKeys,ENCRYPT);

        uint64_t block;

        memcpy(&block, cipher + srcIndex, 8);

        decrypted = encryptedPrev ^ block;
        prevBlock = encryptedPrev;        

        memcpy(dst + dstIndex, &decrypted, 8);

        dstIndex += SIZE_OF_BLOCK_BYTES;
    }

    uint64_t lastBlock;
    memcpy(&lastBlock, dst + dstIndex - SIZE_OF_BLOCK_BYTES, SIZE_OF_BLOCK_BYTES);

    //null tarminating
    dst[dstIndex] = 0;
}

void des_OFB_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP)
        return;

    uint64_t block;
    size_t bytesRead;

    uint64_t prevBlock = generate_random_iv();

    //writing the iv
    fwrite(&prevBlock,sizeof(prevBlock),1,dstP);

    while ((bytesRead = fread(&block, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) != 0)
    {
        uint64_t encryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

        uint64_t encrypted = encryptedPrev ^ block;

        prevBlock = encryptedPrev;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(srcP);
    fclose(dstP);
}

void des_OFB_decrypt_file(const char *cipher, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *cipherP = fopen(cipher, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!cipherP || !dstP)
        return;

    uint64_t currentBlock;
    uint64_t prevBlock;
    size_t bytesRead;

    //reading the iv
    fread(&prevBlock, sizeof(uint64_t), 1, cipherP);

    while (fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t decryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

        prevBlock = decryptedPrev;

        uint64_t decrypted = decryptedPrev ^ currentBlock;

        //no need to habdle padding for this mode beacude of xor
        fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(cipherP);
    fclose(dstP);
}