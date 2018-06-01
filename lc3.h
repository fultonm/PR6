/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
 * 
 *  LC-3 Module Header File
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

#ifndef LC3_H
#define LC3_H

#include "global.h"
#include "alu.h"
#include "cpu.h"
#include "memory.h"

/** FSM microstates */
#define STATE_FETCH 0
#define STATE_DECODE 1
#define STATE_EVAL_ADDR 2
#define STATE_FETCH_OP 3
#define STATE_EXECUTE 4
#define STATE_STORE 5

/** Instruction opcodes */
#define OPCODE_ADD 1    /* 0001 */
#define OPCODE_AND 5    /* 0101 */
#define OPCODE_NOT 9    /* 1001 */
#define OPCODE_BR 0     /* 0000 */
#define OPCODE_JMP 12   /* 1100 */
#define OPCODE_RET 12   /* 1100 */
#define OPCODE_JSR 4    /* 0100 */
#define OPCODE_TRAP 15  /* 1111 */
#define OPCODE_LD 2     /* 0010 */
#define OPCODE_LDI 10   /* 1010 */
#define OPCODE_LDR 6    /* 0110 */
#define OPCODE_LEA 14   /* 1101 */
#define OPCODE_ST 3     /* 0011 */
#define OPCODE_STI 11   /* 1011 */
#define OPCODE_STR 7    /* 0111 */
#define OPCODE_STACK 13 /* 1101 */

/** Stack status codes. Used for the LC-3 stack push/pop opcode */
#define STACK_MAX 0x31F6
#define STACK_BASE 0x31FF
#define STACK_LAST 0x31FE
#define STACK_PUSH 0
#define STACK_POP 1
#define STACK_SUCCESS 1
#define STACK_ERROR 0

/** Mask used to isolating various bits */
#define BIT_IMMED5 16       /* 0000 0000 0001 0000 */
#define BIT_PCOFFSET11 1024 /* 0000 0100 0000 0000 */
#define BIT_PCOFFSET9 256   /* 0000 0001 0000 0000 */
#define BIT_PCOFFSET6 32    /* 0000 0000 0010 0000 */

/** How many times to shift the bits to isolated various values in the IR */
#define BITSHIFT_OPCODE 12
#define BITSHIFT_DR 9
#define BITSHIFT_NZP 9
#define BITSHIFT_SR1 6
#define BITSHIFT_BIT5 5
#define BITSHIFT_BIT11 11
#define BITSHIFT_NEGATIVE_IMMED5 4
#define BITSHIFT_NEGATIVE_PCOFFSET11 10
#define BITSHIFT_NEGATIVE_PCOFFSET9 8
#define BITSHIFT_NEGATIVE_PCOFFSET6 5

#define MASK_OPCODE 61440               // 1111 0000 0000 0000
#define MASK_DR 3584                    // 0000 1110 0000 0000
#define MASK_SR1 448                    // 0000 0001 1100 0000
#define MASK_SR2 7                      // 0000 0000 0000 0111
#define MASK_PCOFFSET11 2047            // 0000 0111 1111 1111
#define MASK_PCOFFSET9 511              // 0000 0001 1111 1111
#define MASK_PCOFFSET6 63               // 0000 0000 0011 1111
#define MASK_TRAPVECT8 255              // 0000 0000 1111 1111
#define MASK_BIT11 2048                 // 0000 1000 0000 0000
#define MASK_BIT5 32                    // 0000 0000 0010 0000
#define MASK_IMMED5 31                  // 0000 0000 0001 1111
#define MASK_NZP 3584                   // 0000 1110 0000 0000
#define MASK_NEGATIVE_IMMED5 0xFFE0     // 1111 1111 1110 0000
#define MASK_NEGATIVE_PCOFFSET11 0xF800 // 1111 1000 0000 0000
#define MASK_NEGATIVE_PCOFFSET9 0xFE00  // 1111 1110 0000 0000
#define MASK_NEGATIVE_PCOFFSET6 0xFFC0  // 1111 1111 1100 0000

typedef struct lc3_t {
    cpu_p cpu;
    alu_p alu;
    memory_p memory;

    word_t starting_address;
    bool_t is_halted;
    bool_t is_file_loaded;

    /** Intra-state variables */
    state_t state;
    opcode_t opcode;
    word_t eval_addr_calculation;
    bool_t branch_enabled;
    trap_vector_t trap_vector;
} lc3_t, *lc3_p;

/** Allocates and initializes a new LC3 module */
lc3_p lc3_create();

/** Reinitializes the LC3 module without reallocation */
void lc3_reset(lc3_p);

/** Deallocates the LC3 module */
void lc3_destroy(lc3_p);

/** Gets a snapshot of the LC3 data for debugging or display purposes */
const lc3_snapshot_t lc3_get_snapshot(lc3_p);

/** Gets/sets the starting address for the PC according to the first line in the loaded hex
 * file */
word_t lc3_get_starting_address(lc3_p);
void lc3_set_starting_address(lc3_p, word_t);

/** Checks/toggles whether the LC3 is halted */
bool_t lc3_is_halted(lc3_p lc3);
void lc3_toggle_halted(lc3_p lc3);

/** Checks/toggles whether the LC3 has a file loaded */
bool_t lc3_has_file_loaded(lc3_p);
void lc3_toggle_file_loaded(lc3_p);

/** Gets/sets the current microstate of the LC3 */
state_t lc3_get_state(lc3_p);
void lc3_set_state(lc3_p, state_t);

/** Gets the opcode currently being processed */
opcode_t lc3_get_opcode(lc3_p);

/** Gets the current PC. This is needed for the Display/debugger to check if the user is
 * requesting a breakpoint at this location */
word_t lc3_get_pc(lc3_p);

/** Sets memory data at the specified address. This is used for loading a file to memory and
 * editing memory from the Display */
void lc3_set_memory(lc3_p, word_t address, word_t data);

/** Fetch instruction cycle */
void lc3_fetch(lc3_p);

/** Decode instruction cycle */
void lc3_decode(lc3_p);

/** The remaining instruction cycles are grouped by operation */
/** ADD */
void lc3_fetch_op_add(lc3_p);
void lc3_execute_add(lc3_p);
void lc3_store_add(lc3_p);

/** AND */
void lc3_fetch_op_and(lc3_p);
void lc3_execute_and(lc3_p);
void lc3_store_and(lc3_p);

/** BR */
void lc3_eval_addr_br(lc3_p);
void lc3_execute_br(lc3_p);

/** JMP */
void lc3_eval_addr_jmp(lc3_p);
void lc3_store_jmp(lc3_p);

/** JSR */
void lc3_eval_addr_jsr(lc3_p);
void lc3_store_jsr(lc3_p);

/** LD */
void lc3_eval_addr_ld(lc3_p);
void lc3_fetch_op_ld(lc3_p);
void lc3_store_ld(lc3_p);

/** LDI */
void lc3_eval_addr_ldi(lc3_p);
void lc3_fetch_op_ldi(lc3_p);
void lc3_store_ldi(lc3_p);

/** LDR */
void lc3_eval_addr_ldr(lc3_p);
void lc3_fetch_op_ldr(lc3_p);
void lc3_store_ldr(lc3_p);

/** LEA */
void lc3_eval_addr_lea(lc3_p);
void lc3_store_lea(lc3_p);

/** NOT */
void lc3_fetch_op_not(lc3_p);
void lc3_execute_not(lc3_p);
void lc3_store_not(lc3_p);

/** ST */
void lc3_eval_addr_st(lc3_p);
void lc3_fetch_op_st(lc3_p);
void lc3_store_st(lc3_p);

/** STI */
void lc3_eval_addr_sti(lc3_p);
void lc3_fetch_op_sti(lc3_p);
void lc3_store_sti(lc3_p);

/** STR */
void lc3_eval_addr_str(lc3_p);
void lc3_fetch_op_str(lc3_p);
void lc3_store_str(lc3_p);

/** Stack */
void lc3_fetch_op_stack(lc3_p);
void lc3_store_stack(lc3_p);

/** TRAP */
void lc3_fetch_op_trap(lc3_p);
word_t lc3_execute_trap(lc3_p);

/** LC3 portion of the GETC routine */
void lc3_trap_x20(lc3_p, char);

/** LC3 portion of the OUT routine */
char lc3_trap_x21(lc3_p);

/** LC3 portion of the PUTS routine */
char lc3_trap_x22(lc3_p);

/** LC3 HALT routine */
void lc3_trap_x25(lc3_p);

#endif
