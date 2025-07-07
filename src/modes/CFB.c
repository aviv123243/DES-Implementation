#include "modes.h"

#include "modes.h"

void des_CFB_encrypt_file(const char *src, const char *dst,uint64_t iv, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *srcP = fopen(src, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!srcP || !dstP) return;

    uint8_t blockBuffer[8];
    size_t bytesRead;

    uint64_t prevBlock = iv;

    while ((bytesRead = fread(blockBuffer, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, srcP)) == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t encryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

        uint64_t block;
        memcpy(&block, blockBuffer, SIZE_OF_BLOCK_BYTES);

        uint64_t encrypted = encryptedPrev ^ block;

        prevBlock = encrypted;

        fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    }

    // Pad the final block
    uint64_t lastBlock = add_padding(blockBuffer, bytesRead);

    //encrypt the prev (CFB)
    uint64_t encryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

    //xor it
    uint64_t encrypted = encryptedPrev ^ lastBlock;
    fwrite(&encrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
    

    fclose(srcP);
    fclose(dstP);
}

void des_CFB_decrypt_file(const char *cipher, const char *dst,uint64_t iv, uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    FILE *cipherP = fopen(cipher, "rb");
    FILE *dstP = fopen(dst, "wb");

    if (!cipherP || !dstP)
        return;

    uint64_t currentBlock, nextBlock;
    uint64_t prevBlock = iv;
    size_t bytesRead;
    
    bytesRead = fread(&currentBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);

    while (bytesRead == SIZE_OF_BLOCK_BYTES)
    {
        uint64_t decryptedPrev = des_block(prevBlock, subKeys, ENCRYPT);

        bytesRead = fread(&nextBlock, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, cipherP);
        
        prevBlock = currentBlock;

        uint64_t decrypted = decryptedPrev ^ currentBlock;

        if (bytesRead < SIZE_OF_BLOCK_BYTES)
        {
            // This is the last block, strip padding
            int padLen = get_padding_len(decrypted);
            fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES - padLen, dstP);
        }
        else
        {
            fwrite(&decrypted, sizeof(uint8_t), SIZE_OF_BLOCK_BYTES, dstP);
        }

        fflush(dstP);

        currentBlock = nextBlock;
    }

    fclose(cipherP);
    fclose(dstP);
}