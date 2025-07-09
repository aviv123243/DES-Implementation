#include "modes.h"

uint64_t build_input_block(uint32_t nonce, uint32_t counter);

int des_CTR_encrypt_string(const char *str, char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    int len = strlen(str);
    int srcIndex = 0;
    int dstIndex = 0;

    uint64_t encrypted;
    uint64_t encryptedPrev;

    uint32_t nonce = generate_random_nonce();
    uint32_t counter = 0;

    memcpy(dst + dstIndex, &nonce, sizeof(nonce));
    dstIndex += NONCE_SIZE_BYTES;

    while (srcIndex < len)
    {
        encryptedPrev = des_block(build_input_block(nonce, counter++), subKeys, ENCRYPT);

        uint64_t block;

        memcpy(&block, str + srcIndex, SIZE_OF_BLOCK_BYTES);

        encrypted = encryptedPrev ^ block;

        memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

        dstIndex += SIZE_OF_BLOCK_BYTES;
        srcIndex += SIZE_OF_BLOCK_BYTES;
    }

    // add it to the ciphertext
    memcpy(dst + dstIndex, &encrypted, SIZE_OF_BLOCK_BYTES);

    // return the length
    return dstIndex + SIZE_OF_BLOCK_BYTES;
}

void des_CTR_decrypt_string(const char *cipher, char *dst, int length, uint64_t key)
{
    uint64_t subKeys[16];

    generate_sub_keys(key, subKeys);

    int srcIndex = 0;
    int dstIndex = 0;

    uint64_t prevBlock;
    uint64_t encryptedInput;
    uint64_t decrypted;

    uint32_t nonce;
    memcpy(&nonce, cipher + srcIndex, sizeof(nonce));

    uint32_t counter = 0;
    srcIndex += NONCE_SIZE_BYTES;

    for (; srcIndex < length; srcIndex += SIZE_OF_BLOCK_BYTES)
    {
        encryptedInput = des_block(build_input_block(nonce, counter++), subKeys, ENCRYPT);

        uint64_t block;

        memcpy(&block, cipher + srcIndex, 8);

        decrypted = encryptedInput ^ block;

        memcpy(dst + dstIndex, &decrypted, 8);

        dstIndex += SIZE_OF_BLOCK_BYTES;
    }

    uint64_t lastBlock;
    memcpy(&lastBlock, dst + dstIndex - SIZE_OF_BLOCK_BYTES, SIZE_OF_BLOCK_BYTES);

    // null tarminating
    dst[dstIndex] = 0;
}

void des_CTR_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP)
        return;

    uint8_t blockBuffer[8] = {0};
    int bytesRead;

    uint32_t nonce = generate_random_nonce();

    // Write the nonce to file
    fwrite(&nonce, sizeof(nonce), 1, dstP);

    uint32_t counter = 0;

    while ((bytesRead = fread(blockBuffer, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t block;
        memcpy(&block, blockBuffer, SIZE_OF_BLOCK_BYTES);

        uint64_t encryptedInput = des_block(build_input_block(nonce, counter++), subKeys, ENCRYPT);
        uint64_t encrypted = encryptedInput ^ block;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    // padd the last block
    uint64_t paddedBlock = add_padding(blockBuffer, bytesRead);
    uint64_t encryptedInput = des_block(build_input_block(nonce, counter++), subKeys, ENCRYPT);
    uint64_t encrypted = encryptedInput ^ paddedBlock;
    fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);

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

    uint64_t currentBlock, nextBlock;
    size_t bytesRead;

    // Read the nonce
    uint32_t nonce;
    fread(&nonce, sizeof(nonce), 1, cipherP);

    uint32_t counter = 0;

    bytesRead = fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

    while (bytesRead == SIZE_OF_BLOCK_BYTES)
    {
        bytesRead = fread(&nextBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

        uint64_t decryptedInput = des_block(build_input_block(nonce, counter++), subKeys, ENCRYPT);
        uint64_t decrypted = decryptedInput ^ currentBlock;

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

uint64_t build_input_block(uint32_t nonce, uint32_t counter)
{
    return ((uint64_t)nonce << 32) | counter;
}