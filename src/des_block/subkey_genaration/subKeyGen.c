#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

    for (int i = 0; i < 28; i++)
    {
        int bit = (key >> (64 - PC1[i])) & 1;
        *Ci |= (bit << (27 - i));
    }

    for (int i = 0; i < 28; i++)
    {
        int bit = (key >> (64 - PC1[i + 28])) & 1;
        *Di |= (bit << (27 - i));
    }
}

uint64_t permuted_choice2(uint32_t Ci, uint32_t Di)
{
    uint64_t CD = (((uint64_t)Ci) << 28) | (Di & 0x0FFFFFFF); // combine into 56-bit
    uint64_t subkey = 0;

    for (int i = 0; i < 48; i++)
    {
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

uint64_t keyGen_next(subKeyGen *skg)
{
    // shift the two halfs
    skg->Ci = circular_left_shift_28(skg->Ci, LEFT_SHIFTS[skg->i]);
    skg->Di = circular_left_shift_28(skg->Di, LEFT_SHIFTS[skg->i]);

    // incrament i
    skg->i++;

    // return the permuted Choice 2
    return permuted_choice2(skg->Ci, skg->Di);
}

void generate_sub_plain_keys(uint64_t key, uint64_t subkeys[NUM_OF_SUBKEYS])
{
    subKeyGen skg;
    keyGen_init(&skg, key);

    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        subkeys[i] = keyGen_next(&skg);
    }
}

// Simple hash to determine child relation
int hash(uint64_t a, uint64_t b) {
    return ((a % 2) == (b % 2)) ? 0 : 1;
}

//generate dag graph based on the keys
//returns 1 if the dag was created and allocated properly
//else 0;
int generate_dag(uint64_t subkeys[NUM_OF_SUBKEYS], nodePtr nodes_out[NUM_OF_SUBKEYS]) {
    for (int i = 0; i < NUM_OF_SUBKEYS; i++) {
        nodes_out[i] = (nodePtr)malloc(sizeof(node));
        if (!nodes_out[i]) {
            perror("Failed to allocate node");
            for (int k = 0; k < i; k++) free(nodes_out[k]);
            return 0;
        }
        nodes_out[i]->val = subkeys[i];
        nodes_out[i]->children = NULL;
        nodes_out[i]->numChildren = 0;
    }

    int childCounts[NUM_OF_SUBKEYS] = {0};
    for (int i = 0; i < NUM_OF_SUBKEYS - 1; i++) {
        for (int j = i + 1; j < NUM_OF_SUBKEYS; j++) {
            if (hash(subkeys[i], subkeys[j]) == 0) {
                childCounts[i]++;
            }
        }
    }

    for (int i = 0; i < NUM_OF_SUBKEYS; i++) {
        if (childCounts[i] > 0) {
            nodes_out[i]->children = (nodePtr *)malloc(sizeof(nodePtr) * childCounts[i]);
            if (!nodes_out[i]->children) {
                perror("Failed to allocate children array");
                for (int k = 0; k <= i; k++) {
                    free(nodes_out[k]->children);
                    free(nodes_out[k]);
                }
                return 0;
            }
        }
    }

    int fillIndex[NUM_OF_SUBKEYS] = {0};
    for (int i = 0; i < NUM_OF_SUBKEYS - 1; i++) {
        for (int j = i + 1; j < NUM_OF_SUBKEYS; j++) {
            if (hash(subkeys[i], subkeys[j]) == 0) {
                nodes_out[i]->children[fillIndex[i]++] = nodes_out[j];
                nodes_out[i]->numChildren++;
            }
        }
    }

    return 1;
}

uint64_t sum_children_values(nodePtr current_node) {
    uint64_t sum = 0;
    for (int i = 0; i < current_node->numChildren; i++) {
        sum += current_node->children[i]->val;
    }
    return sum;
}

void generate_sub_keys_helper(uint64_t subkeys_output[NUM_OF_SUBKEYS], nodePtr nodes_in[NUM_OF_SUBKEYS]) {
    for (int i = 0; i < NUM_OF_SUBKEYS; i++) {
        nodePtr current_node = nodes_in[i];
        uint64_t sum_of_children = sum_children_values(current_node);
        subkeys_output[i] = (current_node->val * sum_of_children) & LAST_48_BITS_MASK;
    }
}

void generate_sub_keys(uint64_t key, uint64_t subkeys[NUM_OF_SUBKEYS]) {
    uint64_t plain_subkeys[NUM_OF_SUBKEYS];
    nodePtr subkey_nodes[NUM_OF_SUBKEYS];

    generate_sub_plain_keys(key, plain_subkeys);

    if (!generate_dag(plain_subkeys, subkey_nodes)) {
        fprintf(stderr, "Error: DAG generation failed.\n");
        memset(subkeys, 0, sizeof(uint64_t) * NUM_OF_SUBKEYS);
        return;
    }

    generate_sub_keys_helper(subkeys, subkey_nodes);

    for (int i = 0; i < NUM_OF_SUBKEYS; i++) {
        free(subkey_nodes[i]->children);
        free(subkey_nodes[i]);
    }
}
