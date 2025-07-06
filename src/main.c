#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "include/des.h"
#include "include/keyGen.h"
#include "include/modes.h"

#include <inttypes.h>



int main() {
    uint8_t tail[] = {'a','b'};

    const char *plaintext = "Hello DES!";
    char encrypted[64] = {0};
    char decrypted[64] = {0};

    uint64_t key = 0x133457799BBCDFF1;

    printf("Plaintext: %s\n", plaintext);

    int newLen = des_ECB_encrypt_string(plaintext, encrypted, key);

    printf("encrypted: ");
    fwrite(encrypted,sizeof(char),newLen,stdout);

    des_ECB_decrypt_string(encrypted, decrypted, newLen, key);

    printf("\nDecrypted: %s\n", decrypted);

    return 0;
}

