#include <stdint.h>
#include <stdlib.h>
#include "include/F.h"
extern const int E[48];
extern const char S[8][64];
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
    uint64_t result = 0;

    for (int i = 0; i < 48; i++) {
        int bit_index = E[i] - 1; 
        int bit = (right >> (31 - bit_index)) & 1; 
        result |= ((uint64_t)bit << (47 - i)); 
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

        // Compute row and column
        int row = ((chunk & 0x20) >> 4) | (chunk & 0x01);  // bits 6 and 1
        int col = (chunk >> 1) & 0x0F;                     // bits 2–5

        // Lookup S-box output (4 bits)
        uint8_t s_out = S[i][row * 16 + col] & 0x0F;

        // Place into result (MSB-first)
        res |= s_out << (28 - 4 * i);
    }

    return res;
}

uint32_t PBox_transposition(uint32_t right)
{
    uint32_t res = 0;
    
    for (int i = 0; i < 32; i++)
    {
        //fetch the next bit according to the P box
        int bit = (right >> (32 - P[i])) & 1;

        //place it
        res |= (bit << (31 - i));
    }

    return res;
    
}