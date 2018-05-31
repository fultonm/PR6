/* 
 * LC-3 Simulator Simulator
 * Contributors: Mike Fulton, Logan Stafford, Enoch Chan
 * TCSS372 - Computer Architecture - Spring 2018
 *
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the
 * 16-bit LC-3 machine based a finite state machine (FSM) interpretation of its
 * operations.
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
#define TRAP_VECTOR_X25 0xF025

/** Allows the Display to edit memory */
void slc3_edit_memory_handler(lc3_p, word_t address, word_t data);

#endif
