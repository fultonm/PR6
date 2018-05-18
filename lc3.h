
#include "global.h"

/** FSM microstates */
#define STATE_FETCH 0
#define STATE_DECODE 1
#define STATE_EVAL_ADDR 2
#define STATE_FETCH_OP 3
#define STATE_EXECUTE 4
#define STATE_STORE 5

/* Instruction opcodes */
#define OPCODE_ADD 1
#define OPCODE_AND 5
#define OPCODE_NOT 9
#define OPCODE_BR 0
#define OPCODE_JMP 12
#define OPCODE_RET 12
#define OPCODE_JSR 4
#define OPCODE_TRAP 15
#define OPCODE_LD 2
#define OPCODE_LDR 6 // 0110 0000 0000 0000
#define OPCODE_LEA 14
#define OPCODE_ST 3
#define OPCODE_STR 7 // 0111 0000 0000 0000

#define BIT_IMMED5 16        // 0000 0000 0001 0000
#define BIT_PCOFFSET11 1024 // 0000 0100 0000 0000
#define BIT_PCOFFSET9 256   // 0000 0001 0000 0000
#define BIT_PCOFFSET6 32    // 0000 0000 0010 0000

// How many times to shift the bits.
#define BITSHIFT_OPCODE 12
#define BITSHIFT_DR 9
#define BITSHIFT_CC 9
#define BITSHIFT_SR1 6
#define BITSHIFT_BIT5 5
#define BITSHIFT_BIT11 11
#define BITSHIFT_CC_BIT3 2
#define BITSHIFT_CC_BIT2 1
#define BITSHIFT_NEGATIVE_IMMED5 4
#define BITSHIFT_NEGATIVE_PCOFFSET11 10
#define BITSHIFT_NEGATIVE_PCOFFSET9 8
#define BITSHIFT_NEGATIVE_PCOFFSET6 5

#define MASK_OPCODE 61440    // 1111 0000 0000 0000
#define MASK_DR 3584         // 0000 1110 0000 0000
#define MASK_SR1 448         // 0000 0001 1100 0000
#define MASK_SR2 7           // 0000 0000 0000 0111
#define MASK_PCOFFSET11 2047 // 0000 0111 1111 1111
#define MASK_PCOFFSET9 511   // 0000 0001 1111 1111
#define MASK_PCOFFSET6 63    // 0000 0000 0011 1111
#define MASK_TRAPVECT8 255   // 0000 0000 1111 1111
#define MASK_BIT11 2048      // 0000 1000 0000 0000
#define MASK_BIT5 32         // 0000 0000 0010 0000
#define MASK_IMMED5 31       // 0000 0000 0001 1111
#define MASK_NZP 3584        // 0000 1110 0000 0000
#define MASK_CC_N 7
#define MASK_CC_Z 5
#define MASK_CC_P 1
#define MASK_NEGATIVE_IMMED5 0xFFE0  // 1111 1111 1110 0000
#define MASK_NEGATIVE_PCOFFSET11 0xF800 // 1111 1000 0000 0000
#define MASK_NEGATIVE_PCOFFSET9 0xFE00  // 1111 1110 0000 0000
#define MASK_NEGATIVE_PCOFFSET6 0xFFC0  // 1111 1111 1100 0000

typedef struct lc3_t *lc3_p;

/** Allocates and initializes a new LC3 module */
lc3_p lc3_create();

/** Reinitializes the LC3 module without reallocation */
void lc3_reset(lc3_p);

/** Deallocates the LC3 module */
void lc3_destroy(lc3_p);

/** Sets the starting address for the PC according to the first line in the loaded hex file */
void lc3_set_starting_address(lc3_p, word_t starting_address);

/** Checks/toggles whether the LC3 is halted */
bool_t lc3_is_halted(lc3_p);
void lc3_toggle_halted(lc3_p);

/** Checks/toggles whether the LC3 has a file loaded */
bool_t lc3_has_file_loaded(lc3_p);
void lc3_toggle_file_loaded(lc3_p);

/** Gets/sets the current microstate of the LC3 */
state_t lc3_get_state(lc3_p);
void lc3_set_state(lc3_p, state_t state);
