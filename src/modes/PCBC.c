#include "modes.h"


void des_PCBC_encrypt_file(const char *src, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP)
        return;

    uint8_t blockBuffer[8];
    size_t bytesRead;

    uint64_t prevBlock = generate_random_iv();

    //writing the iv
    fwrite(&prevBlock,sizeof(prevBlock),1,dstP);

    while ((bytesRead = fread(blockBuffer, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t block;
        memcpy(&block, blockBuffer, SIZE_OF_BLOCK_BYTES);

        block ^= prevBlock;

        uint64_t encrypted = des_block(block, subKeys, ENCRYPT);

        prevBlock = encrypted ^ block;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    if (bytesRead > 0 || feof(srcP))
    {
        // Pad the final block
        uint64_t lastBlock = add_padding(blockBuffer, bytesRead);

        // xor it with prev
        lastBlock ^= prevBlock;

        uint64_t encrypted = des_block(lastBlock, subKeys, ENCRYPT);
        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    fclose(srcP);
    fclose(dstP);
}

void des_PCBC_decrypt_file(const char *cipher, const char *dst, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *cipherP = fopen(cipher, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!cipherP || !dstP)
        return;

    uint64_t currentBlock, nextBlock;
    uint64_t prevBlock;
    int bytesRead;
    int isLast;

    //reading the iv
    fread(&prevBlock, sizeof(uint64_t), 1, cipherP);

    bytesRead = fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

    while (bytesRead == SIZE_OF_BLOCK_BYTES)
    {
        bytesRead = fread(&nextBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

        uint64_t decrypted = des_block(currentBlock, subKeys, DECRYPT);

        uint64_t tempPrev = currentBlock ^ decrypted;

        decrypted ^= prevBlock;

        prevBlock = tempPrev;

        isLast = bytesRead == 0;

        if (isLast)
        {
            fflush(dstP);
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
