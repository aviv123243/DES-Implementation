#pragma once

#include "../../constants.h"

#include <stdint.h>

// PC-1: 64-bit input key → 56-bit output (C0 and D0)
static const int PC1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4};

// PC-2: 56-bit C||D → 48-bit subkey
static const int PC2[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32};

static const int LEFT_SHIFTS[16] = {
    1, 1, 2, 2,
    2, 2, 2, 2,
    1, 2, 2, 2,
    2, 2, 2, 1};

//-----------graph related-----------------

typedef struct nodeType
{
    uint64_t val;
    struct nodeType **children;
    int numChildren;
} node, *nodePtr;

// standard DES subkey generation implementation
void generate_sub_keys_plain(uint64_t key, uint64_t subkeys[NUM_OF_SUBKEYS]);
void generate_sub_keys(uint64_t key, uint64_t subkeys[NUM_OF_SUBKEYS]);
