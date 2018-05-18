/* LC-3 Emulator
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

/* Instruction opcodes */
#define ADD 1
#define AND 5
#define NOT 9
#define BR 0
#define JMP 12
#define RET 12
#define JSR 4
#define TRAP 15
#define LD 2
#define LDR 6 // 0110 0000 0000 0000
#define LEA 14
#define ST 3
#define STR 7 // 0111 0000 0000 0000

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

#define CONDITION_N 4   // 0000 1000 0000 0000
#define CONDITION_Z 2   // 0000 0100 0000 0000
#define CONDITION_P 1   // 0000 0010 0000 0000
#define CONDITION_NZ 6  // 0000 1100 0000 0000
#define CONDITION_NP 5  // 0000 1010 0000 0000
#define CONDITION_ZP 3  // 0000 0110 0000 0000
#define CONDITION_NZP 7 // 0000 1110 0000 0000

#define TRAP_VECTOR_X20 0x20
#define TRAP_VECTOR_X21 0x21
#define TRAP_VECTOR_X22 0x22
#define TRAP_VECTOR_X25 0x25



/* Function Definitions */
void controller(lc3_p);
void lc3_init(lc3_p);
void int16_to_binary_IR_contents(int *, unsigned short);
void trap(unsigned short, lc3_p);
void set_condition_code(int, lc3_p);
void print_binary_form(unsigned int);
void handle_user_input(lc3_p);
void display_display_monitor(lc3_p);
void load_file_to_memory(lc3_p, FILE *);
short SEXT(unsigned short, int);
void setCC(unsigned short, lc3_p);
bool_t branchEnabled(unsigned short, cpu_p);
FILE *open_file2(char *);

#endif
