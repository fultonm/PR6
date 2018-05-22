

#include "global.h"

/** Converts to a 16-bit LC3 memory address from a 0-based array index. Based on the LC3 minimum address */
word_t get_address_from_index(int i) {
    return MEMORY_ADDRESS_MIN + i;
}

/** Convert to a 0-based array index from a 16-bit LC3 memory address */
int get_index_from_address(word_t address) {
    return address - MEMORY_ADDRESS_MIN;
}