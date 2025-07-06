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

    uint64_t block = add_padding(tail,0);

    printf("0x%016" PRIX64 "\n",block);
    const char *plaintext = "Hello DES!";
    char encrypted[64] = {0};
    char decrypted[64] = {0};

    uint64_t key = 0x133457799BBCDFF1;

    printf("Plaintext: %s\n", plaintext);

    des_ECB_encrypt_string(plaintext, encrypted, key);
    des_ECB_decrypt_string(encrypted, decrypted, strlen(plaintext) + SIZE_OF_BLOCK_BYTES, key);

    printf("Decrypted: %s\n", decrypted);

    return 0;
}

