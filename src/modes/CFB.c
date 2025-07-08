#include "modes.h"

#include "modes.h"

void des_CFB_encrypt_file(const char *src, const char *dst, uint64_t key)
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

    while ((bytesRead = fread(&block, sizeof(uint64_t), 1, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t encryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

        uint64_t encrypted = encryptedPrev ^ block;

        prevBlock = encrypted;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

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

    uint64_t currentBlock;
    uint64_t prevBlock;
    size_t bytesRead;

    //reading the iv
    fread(&prevBlock, sizeof(uint64_t), 1, cipherP);

    while (fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t decryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

        prevBlock = currentBlock;

        uint64_t decrypted = decryptedPrev ^ currentBlock;

        //no need to habdle padding for this mode beacude of xor
        fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(cipherP);
    fclose(dstP);
}