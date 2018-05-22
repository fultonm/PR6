

#ifndef MEMORY_H
#define MEMORY_H

#include "global.h"

typedef struct memory_t *memory_p;

/** Allocates and initializes a new memory module. */
memory_p memory_create();

/** Reinitializes the memory module without reallocation */
void memory_reset(memory_p);

/** Deallocates the memory module */
void memory_destroy(memory_p);

/** Takes a snapshot of the memory for debugging or display purposes */
memory_snapshot_t memory_get_snapshot(memory_p);

/** Writes to the specified memory address */
void memory_write(memory_p, word_t address, word_t data);

/** Reads from the specified memory address and returns the data */
word_t memory_get_data(memory_p, word_t);

#endif