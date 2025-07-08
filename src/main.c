#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "modes/modes.h"
#include "key_iv/key_iv.h"
#include "des_block/subkey_genaration/subKeyGen.h"

#include <inttypes.h>

void testFileEncryption(uint64_t key);
void testStringEncryption(uint64_t key);
void testBlockEncryption(uint64_t key);

int main() {

    // uint64_t key = 0x133457799BBCDFF1;

    testStringEncryption(generate_random_key());

    return 0;
}

void testFileEncryption(uint64_t key)
{
    printf("encrypting...\n");

    des_PCBC_encrypt_file("../test/plaintext.txt","../test/ciphertext.txt",key);

    printf("decrypting...\n");

    des_PCBC_decrypt_file("../test/ciphertext.txt","../test/decoded.txt",key);
}

void testStringEncryption(uint64_t key)
{
    const char *plaintext = "DES DES 123";
    char encrypted[64] = {0};
    char decrypted[64] = {0};

    printf("Plaintext: %s\n", plaintext);

    int newLen = des_PCBC_encrypt_string(plaintext, encrypted, key);

    printf("encrypted: ");

    fwrite(encrypted,sizeof(char),newLen,stdout);

    des_PCBC_decrypt_string(encrypted, decrypted, newLen, key);

    printf("\nDecrypted: %s\n", decrypted);
}

void testBlockEncryption(uint64_t key)
{
    uint64_t subKeys[16];
    generate_sub_keys(key, subKeys);

    uint64_t block = 0xAABBCCDD11223344;

    uint64_t cipherBlock = des_block(block, subKeys, ENCRYPT);

    uint64_t decoded = des_block(cipherBlock, subKeys, DECRYPT);

    printf("Plaintext:  0x%016" PRIx64 "\n", block);
    printf("Ciphertext: 0x%016" PRIx64 "\n", cipherBlock);
    printf("decoded: 0x%016" PRIx64 "\n", decoded);

    
}