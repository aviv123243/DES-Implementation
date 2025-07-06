#include <stdint.h>
#include "../modes/modes.h"

uint64_t add_padding(uint8_t tailingBytes[],int numOfBytes)
{
    uint8_t bytes[8] = {0};
    uint64_t res;

    memcpy(bytes, tailingBytes, numOfBytes);  

    uint8_t padValue = SIZE_OF_BLOCK_BYTES - numOfBytes;

    for (int i = numOfBytes; i < SIZE_OF_BLOCK_BYTES; i++) {
        bytes[i] = padValue;  
    }

    memcpy(&res, bytes, SIZE_OF_BLOCK_BYTES);

    return res;
}

int get_padding_len(uint64_t block) {
    uint8_t bytes[8];

    memcpy(bytes,&block,SIZE_OF_BLOCK_BYTES);

    return bytes[SIZE_OF_BLOCK_BYTES - 1];  
}