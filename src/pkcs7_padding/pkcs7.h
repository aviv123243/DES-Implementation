#pragma once

#include <stdint.h>
#include <string.h>

uint64_t add_padding(uint8_t tailingBytes[],int numOfBytes);
int get_padding_len(uint64_t block);