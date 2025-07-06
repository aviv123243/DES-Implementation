#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "include/des.h"
#include "include/keyGen.h"
#include "include/modes.h"

#include <inttypes.h>

int main() {
    uint64_t test = 0xaabbccdd11220000;

    printf("test: 0x%016" PRIx64 "\n",test);
    test = add_padding(test,6);
    printf("pad: 0x%016" PRIx64 "\n",test);
    char *text = "aaaaaaaabbbbbbbbcccccc";
    char encrypted[128] = {0};
    char decrypted[128] = {0};

    uint64_t key = 0x0123456789ABCDEF;

    des_ECB_encrypt_string(text, encrypted, key);

    // assume we know the output size
    des_ECB_decrypt_string(encrypted, decrypted, 24, key); // 3 * 8 = 24 bytes

    printf("Original:  %s\n", text);
    printf("Encrypted:  %s\n", encrypted);
    printf("Decrypted: %s\n", decrypted);

    return 0;
}

