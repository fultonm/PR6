#include "alu.h"
#include "cpu.h"
#include "global.h"
#include "memory.h"

typedef struct lc3_t {
    cpu_p cpu;
    alu_p alu;
    mem_p memory;

    word_t starting_address;
    bool_t is_halted;
    bool_t is_file_loaded;

    /** Intra-state variables */
    state_t state;
    opcode_t opcode;
    reg_addr_t sr1;
    reg_addr_t sr2;
    reg_addr_t dr;
    trap_vector_t trap_vector;
} lc3_t, *lc3_p;

/** Sets LC3 values to default starting values */
void initialize(lc3_p);

/** Reinitializes the variables used during each phase of instruction processing */
void initialize_intrastate(lc3_p);

/** Get the opcode from the IR */
opcode_t get_opcode(lc3_p);

/** Get the destination register from the IR */
reg_addr_t get_dr(lc3_p);

/** Get the source register 1 from the IR */
reg_addr_t get_sr1(lc3_p);

/** Get the source register 2 from the IR */
reg_addr_t get_sr2(lc3_p);

/** Get bit 5 (immediate mode flag for AND and ADD) from the IR */
bool_t get_bit_5(lc3_p);

/** Get the 5 immediate bits (AND and ADD) from the IR */
imm_5_t get_imm_5(lc3_p);

/** Get the 6 PC offset bits (LDR and STR) from the IR */
pc_offset_6_t get_pc_offset_6(lc3_p);

/** Get the 9 PC offset bits (BR, LD, LDI, LEA, ST, and STI) from the IR */
pc_offset_9_t get_pc_offset_9(lc3_p);

/** Get the 11 PC offset bits (JSR) from the IR */
pc_offset_11_t get_pc_offset_11(lc3_p);

/** Get the TRAP vector from the IR */
trap_vector_t get_trap_vector(lc3_p);

/** Allocates and initializes a new LC3 module */
lc3_p lc3_create() {
    lc3_p lc3 = calloc(1, sizeof(lc3_t));
    lc3->cpu = cpu_create();
    lc3->alu = alu_create();
    lc3->memory = memory_create();
    initialize(lc3);
    return lc3;
}

/** Reinitializes the LC3 module without reallocation */
void lc3_reset(lc3_p lc3) {
    cpu_reset(lc3->cpu);
    alu_reset(lc3->alu);
    memory_reset(lc3->memory);
    initialize(lc3);
}

/** Sets LC3 values to default starting values */
void initialize(lc3_p lc3) {
    lc3->starting_address = MEMORY_ADDRESS_MIN;
    lc3->is_halted = FALSE;
    lc3->is_file_loaded = FALSE;
    initialize_intrastate(lc3);
}

/** Reinitializes the variables used during each phase of instruction processing */
void initialize_intrastate(lc3_p lc3) {
    lc3->state = STATE_FETCH;
    lc3->opcode = OPCODE_TRAP;
    lc3->sr1 = R0;
    lc3->sr2 = R0;
    lc3->dr = R0;
    lc3->trap_vector = 0;
}

/** Deallocates the LC3 module */
void lc3_destroy(lc3_p lc3) {
    cpu_destroy(lc3->cpu);
    alu_destroy(lc3->alu);
    memory_destroy(lc3->memory);
    free(lc3);
}

void lc3_ms_18(lc3_p lc3) {
    /** Load the MAR with the contents of the PC, and simultaneously increment the PC. */
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_mar(lc3->cpu, pc);
    cpu_increment_pc(lc3->cpu);
}

void lc3_microstate_33(lc3_p lc3) {
    /** Interrogate memory, resulting in the instruction being placed in the MDR. */
    word_t mar = cpu_get_mar(lc3->cpu);
    word_t data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);
}

void lc3_microstate_35(lc3_p lc3) {
    /** Load the IR with the contents of the MDR. */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_ir(lc3->cpu, mdr);
}

/** The DECODE operation for all operations */
void lc3_decode(lc3_p lc3) {
    /** Corresponding to FSM state 32 and page 106 in the book */

    /** The opcode is calculated from the IR and determines the subsequent path of execution */
    lc3->opcode = get_opcode(lc3);
}

/** EVALUATE ADDRESS operations for each operation */
void lc3_eval_addr_ld(lc3_p lc3) {
    lc3->dr = get_dr(lc3);
    lc3->pc_offset_9 = get_pc_offset_9(lc3);
    word_t offset_pc = 
    cpu_set_mar()
}

/** Sets the starting address for the PC according to the first line in the loaded hex file */
void lc3_set_starting_address(lc3_p lc3, word_t starting_address) {
    lc3->starting_address = starting_address;
}

/** Checks whether the LC3 is halted */
bool_t lc3_is_halted(lc3_p lc3) { return lc3->is_halted; }

/** Toggles whether the LC3 is halted */
void lc3_toggle_halted(lc3_p lc3) { lc3->is_halted = !lc3->is_halted; }

/** Checks whether the LC3 has a file loaded */
bool_t lc3_has_file_loaded(lc3_p lc3) { return lc3->is_file_loaded; }

/** Toggles whether the LC3 has a file loaded */
void lc3_toggle_file_loaded(lc3_p lc3) { lc3->is_file_loaded = !lc3->is_file_loaded; }

/** Gets the current microstate of the LC3 */
state_t lc3_get_state(lc3_p lc3) { return lc3->state; }

/** Sets the current microstate of the LC3 */
void lc3_set_state(lc3_p lc3, state_t state) { lc3->state = state; }

/** Fetches the opcode from the IR */
opcode_t get_opcode(lc3_p lc3) {
    return (opcode_t)(get_ir() & MASK_OPCODE) >> BITSHIFT_OPCODE;
}

/** Fetches the destination register from the IR */
reg_addr_t get_dr(lc3_p lc3);

/** Fetches the source register 1 from the IR */
reg_addr_t get_sr1(lc3_p lc3);

/** Fetches the source register 2 from the IR */
reg_addr_t get_sr2(lc3_p lc3);

/** Fetches bit 5 (immediate mode flag for AND and ADD) from the IR */
bool_t get_bit_5(lc3_p lc3);

/** Fetches the 5 immediate bits (AND and ADD) from the IR */
imm_5_t get_imm_5(lc3_p lc3) {
            imm_5_t masked_ir = (imm_5_t)(cpu_get_ir(lc3->cpu) & MASK_IMMED5);
    if (((masked_ir & BIT_IMMED5) >> BITSHIFT_NEGATIVE_IMMED5) == 1) {
        masked_ir = masked_ir | MASK_NEGATIVE_IMMED5;
    }
    return masked_ir;
}

/** Fetches the 6 PC offset bits (LDR and STR) from the IR */
pc_offset_6_t get_pc_offset_6(lc3_p lc3) {
        pc_offset_6_t masked_ir = (pc_offset_6_t)(cpu_get_ir(lc3->cpu) & MASK_PCOFFSET6);
    if (((masked_ir & BIT_PCOFFSET6) >> BITSHIFT_NEGATIVE_PCOFFSET6) == 1) {
        masked_ir = masked_ir | MASK_NEGATIVE_PCOFFSET6;
    }
    return masked_ir;
}

/** Fetches the 9 PC offset bits (BR, LD, LDI, LEA, ST, and STI) from the IR */
pc_offset_9_t get_pc_offset_9(lc3_p lc3) {
    pc_offset_9_t masked_ir = (pc_offset_9_t)(cpu_get_ir(lc3->cpu) & MASK_PCOFFSET9);
    if (((masked_ir & BIT_PCOFFSET9) >> BITSHIFT_NEGATIVE_PCOFFSET9) == 1) {
        masked_ir = masked_ir | MASK_NEGATIVE_PCOFFSET9;
    }
    return masked_ir;
}

/** Fetches the 11 PC offset bits (JSR) from the IR */
pc_offset_11_t get_pc_offset_11(lc3_p lc3) {
    pc_offset_11_t masked_ir = (pc_offset_11_t)(cpu_get_ir(lc3->cpu) & MASK_PCOFFSET11);
    if (((masked_ir & BIT_PCOFFSET11) >> BITSHIFT_NEGATIVE_PCOFFSET11) == 1) {
        masked_ir = masked_ir | MASK_NEGATIVE_PCOFFSET11;
    }
    return masked_ir;
}

/** Fetches the TRAP vector from the IR */
trap_vector_t get_trap_vector(lc3_p lc3) {

}