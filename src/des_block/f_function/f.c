#include <stdint.h>
#include <stdlib.h>
#include "f.h"
extern const int E[48];
extern const char S[8][4][16];
extern const int P[32];

uint64_t expand(uint32_t right);
uint32_t keyed_substitution(uint64_t right);
uint32_t PBox_transposition(uint32_t right);

uint32_t F(uint32_t right,uint64_t subKey)
{
    // implamenting the mangler function

    uint32_t res;

    uint64_t expandedRight = expand(right);

    expandedRight ^= subKey;

    res = keyed_substitution(expandedRight);

    res = PBox_transposition(res);

    return res;
}

uint64_t expand(uint32_t right)
{
    int expandedSize = 48;
    uint64_t result = 0;

    for (int i = 0; i < expandedSize; i++) {
        int bitIndex = E[i] - 1; 
        int bit = (right >> (SIZE_OF_HALF_BLOCK_BITS - 1 - bitIndex)) & 1; 
        result |= ((uint64_t)bit << (expandedSize - 1 - i)); 
    }

    return result;
}

uint32_t keyed_substitution(uint64_t right)
{
    uint32_t res = 0;

    for (int i = 0; i < 8; i++) {
        // Extract the i-th 6-bit chunk 
        // with the mask 0011 1111 (last 6 bits)
        uint8_t chunk = (right >> (42 - 6 * i)) & 0x3F;

        // Compute row
        // getting bit 6 and moving it to place 2 
        //than connecting with 1
        int row = ((chunk & 0x20) >> 4) | (chunk & 0x01);  

        //compute col 
        // bits 2–5
        int col = (chunk >> 1) & 0x0F;                     

        // Lookup S-box output (4 bits)
        uint8_t s_out = S[i][row][col] & 0x0F;

        // Place into result (MSB-first)
        res |= s_out << (28 - 4 * i);
    }

    return res;
}

uint32_t PBox_transposition(uint32_t right)
{
    uint32_t res = 0;
    
    for (int i = 0; i < SIZE_OF_HALF_BLOCK_BITS; i++)
    {
        //fetch the next bit according to the P box
        int bit = (right >> (SIZE_OF_HALF_BLOCK_BITS - P[i])) & 1;

        //place it
        res |= (bit << (SIZE_OF_HALF_BLOCK_BITS - 1 - i));
    }

    return res;
    
}