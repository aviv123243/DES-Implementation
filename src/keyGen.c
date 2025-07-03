#include <stdint.h>
#include <stdlib.h>
#include "include/keyGen.h"

extern const int PC1[56];
extern const int PC2[48];
extern const int LEFT_SHIFTS[NUM_OF_SUBKEYS];

uint64_t permuted_choice2(uint32_t Ci, uint32_t Di);

typedef struct keyGenType
{
    uint32_t Ci; // 28-bit
    uint32_t Di; // 28-bit
    int i;
} keyGen;

uint32_t circular_left_shift_28(uint32_t chunk, int nshifts)
{
    chunk &= 0x0FFFFFFF;
    return ((chunk << nshifts) | (chunk >> (28 - nshifts))) & 0x0FFFFFFF;
}

void permuted_choice1(uint64_t key, uint32_t *Ci, uint32_t *Di)
{
    *Ci = 0;
    *Di = 0;

    for (int i = 0; i < 28; i++) {
        int bit = (key >> (64 - PC1[i])) & 1;
        *Ci |= (bit << (27 - i));
    }

    for (int i = 0; i < 28; i++) {
        int bit = (key >> (64 - PC1[i + 28])) & 1;
        *Di |= (bit << (27 - i));
    }
}

uint64_t permuted_choice2(uint32_t Ci, uint32_t Di)
{
    uint64_t CD = (((uint64_t)Ci) << 28) | (Di & 0x0FFFFFFF); // combine into 56-bit
    uint64_t subkey = 0;

    for (int i = 0; i < 48; i++) {
        int bit = (CD >> (56 - PC2[i])) & 1;
        subkey |= ((uint64_t)bit << (47 - i));
    }

    return subkey;
}

void keyGen_init(keyGen *kg, uint64_t key)
{
    permuted_choice1(key, &kg->Ci, &kg->Di);  

    kg->i = 0;
}

uint64_t keyGen_next(keyGen *kg) {
    //shift the two halfs
    kg->Ci = circular_left_shift_28(kg->Ci,LEFT_SHIFTS[kg->i]);
    kg->Di = circular_left_shift_28(kg->Di,LEFT_SHIFTS[kg->i]);

    //incrament i
    kg->i++;

    //return the permuted Choice 2
    return permuted_choice2(kg->Ci,kg->Di)   ;
}

void generate_sub_keys(uint64_t key, uint64_t subkeys[16])
{
    keyGen kg;
    keyGen_init(&kg, key);

    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        subkeys[i] = keyGen_next(&kg);  // correctly write into the caller's array
    }
}

