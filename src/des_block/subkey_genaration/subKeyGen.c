#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "subKeyGen.h"

extern const int PC1[56];
extern const int PC2[48];
extern const int LEFT_SHIFTS[16];

uint64_t permuted_choice2(uint32_t Ci, uint32_t Di);

typedef struct subKeyGenType
{
    uint32_t Ci; // 28-bit
    uint32_t Di; // 28-bit
    int i;
} subKeyGen;

uint32_t circular_left_shift_28(uint32_t chunk, int nshifts)
{
    chunk &= LAST_48_BITS_MASK;
    return ((chunk << nshifts) | (chunk >> (SIZE_OF_HALF_KEY_BITS - nshifts))) & LAST_48_BITS_MASK;
}

void permuted_choice1(uint64_t key, uint32_t *Ci, uint32_t *Di)
{
    *Ci = 0;
    *Di = 0;

    for (int i = 0; i < SIZE_OF_HALF_KEY_BITS; i++)
    {
        int bit = (key >> (SIZE_OF_BLOCK_BITS - PC1[i])) & 1;
        *Ci |= (bit << (SIZE_OF_HALF_KEY_BITS - 1 - i));
    }

    for (int i = 0; i < SIZE_OF_HALF_KEY_BITS; i++)
    {
        int bit = (key >> (SIZE_OF_BLOCK_BITS - PC1[i + SIZE_OF_HALF_KEY_BITS])) & 1;
        *Di |= (bit << (SIZE_OF_HALF_KEY_BITS - 1 - i));
    }
}

uint64_t permuted_choice2(uint32_t Ci, uint32_t Di)
{
    uint64_t CD = (((uint64_t)Ci) << SIZE_OF_HALF_KEY_BITS) | (Di & LAST_48_BITS_MASK); // combine into 56-bit
    uint64_t subkey = 0;

    for (int i = 0; i < SIZE_OF_SUBKEY_BITS; i++)
    {
        int bit = (CD >> (SIZE_OF_EFFECTIVE_KEY_BITS - PC2[i])) & 1;
        subkey |= ((uint64_t)bit << (SIZE_OF_SUBKEY_BITS - 1 - i));
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

void generate_sub_keys_plain(uint64_t key, uint64_t subkeys[NUM_OF_SUBKEYS])
{
    subKeyGen skg;
    keyGen_init(&skg, key);

    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        subkeys[i] = keyGen_next(&skg);
    }
}

// Simple hash to determine child relation
int hash(uint64_t a, uint64_t b)
{
    return ((a % 7) == (b % 13)) ? 0 : 1;
}

void init_graph(uint64_t subkeys[NUM_OF_SUBKEYS], nodePtr nodes_out[NUM_OF_SUBKEYS])
{
    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        nodes_out[i] = (nodePtr)malloc(sizeof(node));
        if (!nodes_out[i])
        {
            perror("Failed to allocate node");
            for (int k = 0; k < i; k++)
                free(nodes_out[k]);

            exit(-1);
        }

        nodes_out[i]->val = subkeys[i];
        nodes_out[i]->children = NULL;
        nodes_out[i]->numChildren = 0;
    }
}

void free_graph(nodePtr subkey_nodes[NUM_OF_SUBKEYS])
{
    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        free(subkey_nodes[i]->children);
        free(subkey_nodes[i]);
    }
}

// generate dag graph based on the keys
// returns 1 if the dag was created and allocated properly
// else 0;
void generate_graph(uint64_t subkeys[NUM_OF_SUBKEYS], nodePtr nodes_out[NUM_OF_SUBKEYS])
{
    init_graph(subkeys, nodes_out);

    int childCounts[NUM_OF_SUBKEYS] = {0};

    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        for (int j = 0; j < NUM_OF_SUBKEYS; j++)
        {
            if (hash(subkeys[i], subkeys[j]) == 0)
            {
                childCounts[i]++;
            }
        }
    }

    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        if (childCounts[i] > 0)
        {
            nodes_out[i]->children = (nodePtr *)malloc(sizeof(nodePtr) * childCounts[i]);
            if (!nodes_out[i]->children)
            {
                perror("Failed to allocate children array");
                for (int k = 0; k <= i; k++)
                {
                    free(nodes_out[k]->children);
                    free(nodes_out[k]);
                }

                exit(-1);
            }
        }
    }

    int fillIndex[NUM_OF_SUBKEYS] = {0};
    for (int i = 0; i < NUM_OF_SUBKEYS - 1; i++)
    {
        for (int j = i + 1; j < NUM_OF_SUBKEYS; j++)
        {
            if (hash(subkeys[i], subkeys[j]) == 0)
            {
                nodes_out[i]->children[fillIndex[i]++] = nodes_out[j];
                nodes_out[i]->numChildren++;
            }
        }
    }
}

uint64_t sum_children_values(nodePtr current_node)
{
    uint64_t sum = 0;
    for (int i = 0; i < current_node->numChildren; i++)
    {
        sum += current_node->children[i]->val;
    }
    return sum;
}

void defuse_subkeys(uint64_t subkeys_output[NUM_OF_SUBKEYS], nodePtr nodes_in[NUM_OF_SUBKEYS])
{
    for (int i = 0; i < NUM_OF_SUBKEYS; i++)
    {
        nodePtr current_node = nodes_in[i];

        uint64_t sum_of_children = sum_children_values(current_node);

        subkeys_output[i] = (current_node->val * sum_of_children) & LAST_48_BITS_MASK;
    }
}

void generate_sub_keys(uint64_t key, uint64_t subkeys[NUM_OF_SUBKEYS])
{
    uint64_t plain_subkeys[NUM_OF_SUBKEYS];
    nodePtr subkey_nodes[NUM_OF_SUBKEYS];

    generate_sub_keys_plain(key, plain_subkeys);

    generate_graph(plain_subkeys, subkey_nodes);

    defuse_subkeys(subkeys, subkey_nodes);

    free_graph(subkey_nodes);
}
