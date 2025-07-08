#include "modes.h"

uint64_t build_input_block(uint32_t nonce,uint32_t counter);

void des_CTR_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP)
        return;

    uint64_t block;
    int bytesRead;

    uint32_t nonce = generate_random_nonce();

    //writing the Nonce to file 
    fwrite(&nonce,sizeof(nonce),1,dstP);

    uint32_t counter = 0;

    while ((bytesRead = fread(&block, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t incryptedInput = des_block(build_input_block(nonce,counter++), subKeys, ENCRYPT);

        uint64_t encrypted = incryptedInput ^ block;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(srcP);
    fclose(dstP);
}

void des_CTR_decrypt_file(const char *cipher, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *cipherP = fopen(cipher, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!cipherP || !dstP)
        return;

    uint64_t currentBlock;
    size_t bytesRead;

    //reading the nonce from file
    uint32_t nonce;
    fread(&nonce,sizeof(nonce),1,cipherP);

    uint32_t counter = 0;

    while (fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t decryptedInput = des_block(build_input_block(nonce,counter++), subKeys, ENCRYPT);

        uint64_t decrypted = decryptedInput ^ currentBlock;

        //no need to habdle padding for this mode beacuse of xor
        fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(cipherP);
    fclose(dstP);
}

uint64_t build_input_block(uint32_t nonce,uint32_t counter)
{
    return ((uint64_t)nonce << 32) | counter;
}