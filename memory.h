#include "global.h"

typedef struct memory_t *mem_p;

/** Allocates and initializes a new memory module. */
mem_p memory_create();

/** Reinitializes the memory module without reallocation */
void memory_reset(mem_p);

/** Deallocates the memory module */
void memory_destroy(mem_p);

/** Writes to the specified memory address */
void memory_write(mem_p, word_t address, word_t data);

/** Reads from the specified memory address and returns the data */
word_t memory_get_data(mem_p, word_t address);
