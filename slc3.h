/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
 * 
 *  LC-3 Simulator Module Header File
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

#ifndef SLC3_H
#define SLC3_H

#include "global.h"
#include "lc3.h"
#include <stdio.h>

/* Other Program Constants */
#define NUM_OF_BITS 16
#define NUM_OF_INSTRUCTIONS 6
#define NUM_OF_REGISTERS 8

#define FILENAME_SIZE 200
#define STRING_SIZE 200
#define OUTPUT_LINE_NUMBER 24
#define OUTPUT_COL_NUMBER 8
#define OUTPUT_AREA_DEPTH 6

#define MAX_HEX_BITS 4
#define MAX_BIN_BITS 16

/* Condition Code Values */
#define CONDITION_N 4   // 0000 1000 0000 0000
#define CONDITION_Z 2   // 0000 0100 0000 0000
#define CONDITION_P 1   // 0000 0010 0000 0000
#define CONDITION_NZ 6  // 0000 1100 0000 0000
#define CONDITION_NP 5  // 0000 1010 0000 0000
#define CONDITION_ZP 3  // 0000 0110 0000 0000
#define CONDITION_NZP 7 // 0000 1110 0000 0000

/* Trap Vector Values */
#define TRAP_VECTOR_X20 0x20
#define TRAP_VECTOR_X21 0x21
#define TRAP_VECTOR_X22 0x22
#define TRAP_VECTOR_X25 0x25

/** Allows the Display to edit memory */
void slc3_edit_memory_handler(lc3_p, word_t address, word_t data);

#endif
