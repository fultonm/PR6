#include "global.h"

typedef struct memory_t *memory_p;

/** Allocates and initializes a new memory module. */
memory_p memory_create();

/** Reinitializes the memory module without reallocation */
void memory_reset(memory_p);

/** Deallocates the memory module */
void memory_destroy(memory_p);

/** Writes to the specified memory address */
void memory_write(memory_p, word_t address, word_t data);

/** Reads from the specified memory address and returns the data */
word_t memory_get_data(memory_p, word_t address);

/** Takes a snapshot of the current memory state (Todo: I don't see an alternative to this. The display monitor needs to see all the data of the memory, but this is hidden by the LC3's OOP prinicpal) */
const memory_p memory_get_const_ptr(memory_p);
