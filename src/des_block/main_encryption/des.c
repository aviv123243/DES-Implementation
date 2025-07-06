#include <stdlib.h>
#include <stdint.h>
#include "des.h"
#include "../f_function/f.h"
#include "../subkey_genaration/subKeyGen.h"

uint64_t permute(uint64_t block, const int table[64])
{
    uint64_t res = 0;

    for (int i = 0; i < 64; i++)
    {
        int bit = (block >> (64 - table[i])) & 1;
        res |= ((uint64_t)bit << (63 - i));
    }

    return res;
}

uint64_t des_block(uint64_t block, uint64_t subkeys[], Operation operation)
{
    block = permute(block, IP);

    uint32_t R = block & L64_MASK;
    uint32_t L = (block & H64_MASK) >> 32;
    uint32_t RCopy;
    int subKeyIndex;

    for (int i = 0; i < 16; i++)
    {
        subKeyIndex = (operation == ENCRYPT) ? i : 15 - i;

        RCopy = R;

        R = F(R, subkeys[subKeyIndex]) ^ L;

        L = RCopy;
    }

    block = ((uint64_t)R << 32) | L;

    return permute(block, PI);
}
