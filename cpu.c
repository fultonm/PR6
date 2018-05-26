
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "cpu.h"

#define MASK_WORD_T_HIGH_ORDER 32768 /* 1000 0000 0000 0000 */
#define BITSHIFT_HIGH_ORDER 15

/* CPU Struct */
typedef struct cpu_t {
    word_t registers[REGISTER_SIZE];
    cc_t cc;
    word_t ir;  // instruction_register
    word_t pc;  // program_counter
    word_t mar; // memory_address_register
    word_t mdr; // memory_data_register
} cpu_t, *cpu_p;

/** Initializes the variables of the CPU */
void initialize_cpu(cpu_p);

/** Sets the CPU based on the value just written to a register */
void set_cc(cpu_p, word_t);

/** Creates a CPU object, initializes it, and returns the pointer */
cpu_p cpu_create() {
    cpu_p cpu = calloc(1, sizeof(cpu_t));
    initialize_cpu(cpu);
    return cpu;
}

/** Reinitializes the CPU object without reallocation */
void cpu_reset(cpu_p cpu) { initialize_cpu(cpu); }

/** Deallocates the CPU object */
void cpu_destroy(cpu_p cpu) { free(cpu); }

/** Takes a snapshot of the CPU data for debugging or display purposes */
const cpu_snapshot_t cpu_get_snapshot(cpu_p cpu) {
    cpu_snapshot_t snapshot;
    snapshot.cc_n = cpu_get_cc_n(cpu);
    snapshot.cc_z = cpu_get_cc_z(cpu);
    snapshot.cc_p = cpu_get_cc_p(cpu);
    snapshot.pc = cpu->pc;
    snapshot.ir = cpu->ir;
    snapshot.mar = cpu->mar;
    snapshot.mdr = cpu->mdr;
    memcpy(snapshot.registers, cpu->registers, sizeof(word_t) * REGISTER_SIZE);
    return snapshot;
}

/** Fetches the data at the specified regiser */
word_t cpu_get_register(cpu_p cpu, reg_addr_t reg) { return cpu->registers[reg]; }

/** Sets the specified register to the given data and sets the CC */
void cpu_set_register(cpu_p cpu, reg_addr_t reg, word_t data) {
    cpu->registers[reg] = data;
    set_cc(cpu, data);
}

/** Fetches the high order bit of the CC representing negative */
bool_t cpu_get_cc_n(cpu_p cpu) {
    /** No masking needed */
    return (bool_t)(cpu->cc >> BITSHIFT_CC_N);
}

/** Fetches the second bit of the CC representing zero */
bool_t cpu_get_cc_z(cpu_p cpu) {
    /** Mask and shift */
    return (bool_t)((cpu->cc & MASK_CC_Z) >> BITSHIFT_CC_Z);
}

/** Fetches the low order bit of the CC representing positive */
bool_t cpu_get_cc_p(cpu_p cpu) {
    /** No shifting needed */
    return cpu->cc & MASK_CC_P;
}

/** Sets the instruction register */
void cpu_set_ir(cpu_p cpu, word_t data) { cpu->ir = data; }

/** Gets the instruction register */
word_t cpu_get_ir(cpu_p cpu) { return cpu->ir; }

/** Increments the PC by 1 and returns */
void cpu_increment_pc(cpu_p cpu) { cpu->pc++; }

/** Increments/decrements the PC by the given value */
void cpu_increment_pc_by_value(cpu_p cpu, word_t value) { cpu->pc += value; }

/** Sets the PC to a new value */
void cpu_set_pc(cpu_p cpu, word_t data) {
    cpu->pc = data;
}

/** Returns the current PC */
word_t cpu_get_pc(cpu_p cpu) { return cpu->pc; }

/** Returns the current MAR */
word_t cpu_get_mar(cpu_p cpu) { return cpu->mar; }

/** Sets the MAR */
void cpu_set_mar(cpu_p cpu, word_t data) { cpu->mar = data; }

/** Returns the current MDR */
word_t cpu_get_mdr(cpu_p cpu) { return cpu->mdr; }

/** Sets the MDR */
void cpu_set_mdr(cpu_p cpu, word_t data) { cpu->mdr = data; }

void initialize_cpu(cpu_p cpu) {
    size_t i;
    for (i = 0; i < REGISTER_SIZE; i++) {
        cpu->registers[i] = 0;
    }
    cpu->cc = 0;
    cpu->ir = 0;
    cpu->pc = 0;
    cpu->mar = 0;
    cpu->mdr = 0;
}

/** Sets the CPU based on the value just written to a register */
void set_cc(cpu_p cpu, word_t value) {
    word_t masked_value = value & MASK_WORD_T_HIGH_ORDER;
    bool_t negative = masked_value >> BITSHIFT_HIGH_ORDER;
    if (negative == TRUE) {
        cpu->cc = 4;
    } else if (value == 0) {
        cpu->cc = 2;
    } else {
        cpu->cc = 1;
    }
}