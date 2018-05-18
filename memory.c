#include "memory.h"
#include "slc3.h"
#include <stdlib.h>

/** The time (in milliseconds) the memory will take to read and write */
#define MEM_WRITE_DELAY 50
#define MEM_READ_DELAY 50

typedef struct memory_t {
    word_t data[MEMORY_SIZE];
} memory_t, *mem_p;

/** Initializes each memory location to zero */
void initialize(mem_p);

size_t address_to_index(word_t);

word_t index_to_address(size_t);

/** Allocates and initializes a new memory module. */
mem_p memory_create() {
    mem_p memory = calloc(MEMORY_SIZE, sizeof(memory_t));
    initialize(memory);
    return memory;
}
/** Reinitializes the memory module without reallocation */
void memory_reset(mem_p memory) { initialize(memory); }

/** Initializes each memory location to zero */
void initialize(mem_p memory) {
    unsigned int i;
    for (i = 0; i < MEMORY_SIZE; i++) {
        memory->data[i] = 0;
    }
}

/** Deallocates the memory module */
void memory_destroy(mem_p memory) { free(memory); }

/** Writes to the specified memory address */
void memory_write(mem_p memory, word_t address, word_t data) {
    size_t index = address_to_index(address);
    memory->data[index] = data;
}

/** Reads from the memory at the specified address and returns the data */
word_t memory_get_data(mem_p memory, word_t address) {
    size_t index = address_to_index(address);
    return memory->data[index];
}

size_t address_to_index(word_t address) { return address - MEMORY_ADDRESS_MIN; }

word_t index_to_address(size_t index) { return MEMORY_ADDRESS_MIN + index; }