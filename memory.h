/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
 * 
 *  Memory Module Header File
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
const memory_snapshot_t memory_get_snapshot(memory_p);

/** Writes to the specified memory address */
void memory_write(memory_p, word_t address, word_t data);

/** Reads from the specified memory address and returns the data */
word_t memory_get_data(memory_p, word_t);

#endif
