/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
 * 
 *  Global Module File
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

#include "global.h"
#include "stdlib.h"
#include "string.h"

/** Converts to a 16-bit LC3 memory address from a 0-based array index. Based on the LC3
 * minimum address */
word_t get_address_from_index(int i) { return MEMORY_ADDRESS_MIN + i; }

/** Convert to a 0-based array index from a 16-bit LC3 memory address */
int get_index_from_address(word_t address) { return address - MEMORY_ADDRESS_MIN; }

/** Returns a 16 bit LC3 word parsed from the specified string */
word_t get_word_from_string(char *str) {
    /* A little bit of edge case handling...
     * x3002 + strlen("x3002") - 4 = 3002
     * 3002 + strlen("3002") - 4 = 3002 */
    word_t word = strtol(str + strlen(str) - 4, NULL, 16);
    return (word_t)word;
}