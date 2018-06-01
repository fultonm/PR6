/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
 *
 *  Memory Module File
 * 
 *  This is a simulator of the LC-3 (Little Computer) machine using an 
 *  object-oriented approach in C. The simulator includes all standard LC-3 
 *  functionality based on the finite state machine approach and the corresponding
 *  opcode tables for the machine, with an additional push-pop stack feature utilized 
 *  on the previously reserved (1101) opcode.
 * 
 *  Group Members:
 *  Michael Fulton
 *  Enoch Chan
 *  Logan Stafford
 * 
 *  Base Code Contributors:
 *  Sam Brendel
 *  Michael Josten
 *  Sam Anderson
 *  Tyler Schupack  
 */

#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** The time (in milliseconds) the memory will take to read and write */
#define MEM_WRITE_DELAY 50
#define MEM_READ_DELAY 50

typedef struct memory_t {
    word_t data[MEMORY_SIZE];
} memory_t, *memory_p;

/** Initializes each memory location to zero */
void initialize_memory(memory_p);

size_t address_to_index(word_t);

word_t index_to_address(size_t);

/** Allocates and initializes a new memory module. */
memory_p memory_create() {
    memory_p memory = calloc(MEMORY_SIZE, sizeof(memory_t));
    initialize_memory(memory);
    return memory;
}
/** Reinitializes the memory module without reallocation */
void memory_reset(memory_p memory) { initialize_memory(memory); }

/** Deallocates the memory module */
void memory_destroy(memory_p memory) { free(memory); }

/** Takes a snapshot of the memory for debugging or display purposes */
const memory_snapshot_t memory_get_snapshot(memory_p memory) {
    memory_snapshot_t snapshot;
    memcpy(snapshot.data, memory->data, sizeof(word_t) * MEMORY_SIZE);
    return snapshot;
}

/** Writes to the specified memory address */
void memory_write(memory_p memory, word_t address, word_t data) {
    size_t index = address_to_index(address);
    memory->data[index] = data;
}

/** Reads from the memory at the specified address and returns the data */
word_t memory_get_data(memory_p memory, word_t address) {
    size_t index = address_to_index(address);
    return memory->data[index];
}

/** Initializes each memory location to zero */
void initialize_memory(memory_p memory) {
    unsigned int i;
    for (i = 0; i < MEMORY_SIZE; i++) {
        memory->data[i] = 0;
    }
}

size_t address_to_index(word_t address) { return address - MEMORY_ADDRESS_MIN; }

word_t index_to_address(size_t index) { return MEMORY_ADDRESS_MIN + index; }
