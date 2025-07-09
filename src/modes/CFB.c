#include "modes.h"

int des_CFB_encrypt_string(const char *str, char *dst, uint64_t key)
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
        prevBlock = encrypted;

        memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

        dstIndex += SIZE_OF_BLOCK_BYTES;
        srcIndex += SIZE_OF_BLOCK_BYTES;
    }

    // add it to the ciphertext
    memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

    // return the length        
    return dstIndex + SIZE_OF_BLOCK_BYTES;
}

void des_CFB_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
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
        prevBlock = block;        

        memcpy(dst + dstIndex, &decrypted, 8);

        dstIndex += SIZE_OF_BLOCK_BYTES;
    }

    uint64_t lastBlock;
    memcpy(&lastBlock, dst + dstIndex - SIZE_OF_BLOCK_BYTES, SIZE_OF_BLOCK_BYTES);

    //null tarminating
    dst[dstIndex] = 0;
}

void des_CFB_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP)
        return;

    uint8_t blockBuffer[8] = {0};
    size_t bytesRead;

    uint64_t prevBlock = generate_random_iv();

    // Write IV
    fwrite(&prevBlock, sizeof(prevBlock), 1, dstP);

    while ((bytesRead = fread(blockBuffer, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t block;
        memcpy(&block, blockBuffer, SIZE_OF_BLOCK_BYTES);

        uint64_t encryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);
        uint64_t encrypted = encryptedPrev ^ block;

        prevBlock = encrypted;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    // pad the last block
    uint64_t paddedBlock = add_padding(blockBuffer, bytesRead);
    uint64_t encryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);
    uint64_t encrypted = encryptedPrev ^ paddedBlock;
    fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);

    fclose(srcP);
    fclose(dstP);
}

void des_CFB_decrypt_file(const char *cipher, const char *dst, uint64_t key)
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

    // Read IV
    fread(&prevBlock, sizeof(uint64_t), 1, cipherP);

    bytesRead = fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

    while (bytesRead == SIZE_OF_BLOCK_BYTES)
    {
        bytesRead = fread(&nextBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

        uint64_t decryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);
        uint64_t decrypted = decryptedPrev ^ currentBlock;

        prevBlock = currentBlock;

        int isLast = (bytesRead != SIZE_OF_BLOCK_BYTES);

        if (isLast)
        {
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

