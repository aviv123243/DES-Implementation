#include <stdint.h>
#include <stdlib.h>
#include "subKeyGen.h"

extern const int PC1[56];
extern const int PC2[48];
extern const int LEFT_SHIFTS[NUM_OF_SUBKEYS];

uint64_t permuted_choice2(uint32_t Ci, uint32_t Di);

typedef struct subKeyGenType
{
    uint32_t Ci; // 28-bit
    uint32_t Di; // 28-bit
    int i;
} subKeyGen;

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

void keyGen_init(subKeyGen *skg, uint64_t key)
{
    permuted_choice1(key, &skg->Ci, &skg->Di);  

    skg->i = 0;
}

uint64_t keyGen_next(subKeyGen *skg) {
    //shift the two halfs
    skg->Ci = circular_left_shift_28(skg->Ci,LEFT_SHIFTS[skg->i]);
    skg->Di = circular_left_shift_28(skg->Di,LEFT_SHIFTS[skg->i]);

    //incrament i
    skg->i++;

    //return the permuted Choice 2
    return permuted_choice2(skg->Ci,skg->Di)   ;
}

void generate_sub_keys(uint64_t key, uint64_t subkeys[16])
{
    subKeyGen skg;
    keyGen_init(&skg, key);

    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        subkeys[i] = keyGen_next(&skg);  
    }
}

