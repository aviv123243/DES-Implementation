#include <stdlib.h>
#include <stdint.h>
#include "des.h"


uint64_t permute(uint64_t block, const int table[64])
{
    uint64_t res = 0;

    for (int i = 0; i < SIZE_OF_BLOCK_BITS; i++)
    {
        int bit = (block >> (SIZE_OF_BLOCK_BITS - table[i])) & 1;
        res |= ((uint64_t)bit << (SIZE_OF_BLOCK_BITS - 1 - i));
    }

    return res;
}

uint64_t des_block(uint64_t block, uint64_t subkeys[], Operation operation)
{
    block = permute(block, IP);

    uint32_t R = block & L64_MASK;
    uint32_t L = (block & H64_MASK) >> SIZE_OF_HALF_BLOCK_BITS;
    uint32_t RCopy;
    int subKeyIndex;

    for (int i = 0; i < NUM_ROUNDS; i++)
    {
        subKeyIndex = (operation == ENCRYPT) ? i : NUM_ROUNDS - 1 - i;

        RCopy = R;

        R = F(R, subkeys[subKeyIndex]) ^ L;

        L = RCopy;
    }

    block = ((uint64_t)R << SIZE_OF_HALF_BLOCK_BITS) | L;

    return permute(block, PI);
}
