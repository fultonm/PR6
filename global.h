
/** Constants for representing boolean value */
#define FALSE 0
#define TRUE 1

/** Configuration constants */
#define REGISTER_SIZE 8
#define MEMORY_SIZE 512
#define MEMORY_ADDRESS_MIN 0x3000

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

typedef struct cpu_snapshot_t {
    word_t registers[REGISTER_SIZE];
    cc_t cc;
    word_t ir;
    word_t pc;
    word_t mar;
    word_t mdr;
} cpu_snapshot_t;

typedef struct alu_snapshot_t {
    word_t a;
    word_t b;
    word_t result;
} alu_snapshot_t;

typedef struct memory_snapshot_t {
    word_t data[MEMORY_SIZE];
} memory_snapshot_t;

typedef struct lc3_snapshot_t {
    word_t starting_address;
    cpu_snapshot_t cpu_snapshot;
    alu_snapshot_t alu_snapshot;
    memory_snapshot_t memory_snapshot;
} lc3_snapshot_t;

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
