#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "modes/modes.h"

#include <inttypes.h>

void testFileEncryption(uint64_t key);
void testStringEncryption(uint64_t key);


int main() {

    uint64_t key = 0x133457799BBCDFF1;

    testFileEncryption(key);
    

    return 0;
}

void testFileEncryption(uint64_t key)
{
    printf("encrypting...\n");

    des_ECB_encrypt_file("../test/plaintext.txt","../test/ciphertext.txt",key);

    printf("decrypting...\n");

    des_ECB_decrypt_file("../test/ciphertext.txt","../test/decoded.txt",key);
}

void testStringEncryption(uint64_t key)
{
    const char *plaintext = "Hello DES!";
    char encrypted[64] = {0};
    char decrypted[64] = {0};

    printf("Plaintext: %s\n", plaintext);

    int newLen = des_ECB_encrypt_string(plaintext, encrypted, key);

    printf("encrypted: ");

    fwrite(encrypted,sizeof(char),newLen,stdout);

    des_ECB_decrypt_string(encrypted, decrypted, newLen, key);

    printf("\nDecrypted: %s\n", decrypted);
}