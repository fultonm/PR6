

#ifndef GLOBAL_H
#define GLOBAL_H

/** Constants for representing boolean value */
#define FALSE 0
#define TRUE 1

/** Configuration constants */
#define REGISTER_SIZE 8
#define MEMORY_SIZE 512
#define MEMORY_ADDRESS_MIN 0x3000
#define STACK_MAX 0x3200
#define STACK_BASE 0x320A

/** Register address indicies */
#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7

#define MASK_CC_Z 5 /* 010 */
#define MASK_CC_P 1 /* 001 */

#define BITSHIFT_CC_N 2
#define BITSHIFT_CC_Z 1

/** Define the data types used by LC-3. Each LC-3 data type is assigned the smallest and most
 * appropriate C data type here. */
typedef unsigned short word_t;
typedef unsigned char bool_t;
typedef unsigned char state_t;
typedef unsigned char opcode_t;
typedef unsigned char cc_t;
typedef signed char imm_5_t;
typedef signed char pc_offset_6_t;
typedef signed short pc_offset_9_t;
typedef signed short pc_offset_11_t;
typedef unsigned char trap_vector_t;
typedef unsigned char reg_addr_t;

/** Snapshot of the CPU. See LC3 snapshot comment */
typedef struct cpu_snapshot_t cpu_snapshot_t;
struct cpu_snapshot_t {
    word_t registers[REGISTER_SIZE];
    bool_t cc_n;
    bool_t cc_z;
    bool_t cc_p;
    word_t ir;
    word_t pc;
    word_t mar;
    word_t mdr;
};

/** Snapshot of the ALU. See LC3 snapshot comment */
typedef struct alu_snapshot_t alu_snapshot_t;
struct alu_snapshot_t {
    word_t a;
    word_t b;
    word_t result;
};

/** Snapshot of the memory. See LC3 snapshot comment */
typedef struct memory_snapshot_t memory_snapshot_t;
struct memory_snapshot_t {
    word_t data[MEMORY_SIZE];
};

/** The LC3 snapshot contains all the data for the LC3 at a specific moment in time. This
 * compilation of data allows for debugging and/or displaying of the LC3 contents at specific
 * moments in time. It may also be useful for recreating the state of the LC3 if it needed to
 * be interrupted, cleared, then restored, or put to sleep and awoken later */
typedef struct lc3_snapshot_t lc3_snapshot_t;
struct lc3_snapshot_t {
    word_t starting_address;
    bool_t file_loaded;
    bool_t is_halted;
    cpu_snapshot_t cpu_snapshot;
    alu_snapshot_t alu_snapshot;
    memory_snapshot_t memory_snapshot;
};

/** Converts to a 16-bit LC3 memory address from a 0-based array index. Based on the LC3 minimum address */
word_t get_address_from_index(int);

/** Convert to a 0-based array index from a 16-bit LC3 memory address */
int get_index_from_address(word_t);

#endif
