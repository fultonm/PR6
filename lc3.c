
#include "lc3.h"
#include "alu.h"
#include "cpu.h"
#include "global.h"
#include "memory.h"
#include <stdlib.h>

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

/** Sets LC3 values to default starting values */
void initialize_lc3(lc3_p);

/** Reinitializes the variables used during each phase of instruction processing */
void initialize_intrastate(lc3_p);

/** Fetches the opcode from the IR */
opcode_t fetch_opcode(lc3_p);

/** Fetches the destination register from the IR */
reg_addr_t get_dr(lc3_p);

/** Fetches the source register 1 from the IR */
reg_addr_t get_sr1(lc3_p);

/** Fetches the source register 2 from the IR */
reg_addr_t get_sr2(lc3_p);

/** Fetches bit 5 (immediate mode flag for AND and ADD) from the IR */
bool_t get_imm_mode(lc3_p);

/** Fetches the immediate bits from the IR */
bool_t get_jsr_imm_mode(lc3_p);

/** Fetches the NZP bits from the IR */
cc_t get_nzp(lc3_p);

/** Fetches the high order bit of the NZP representing negative */
bool_t get_nzp_n(cc_t);

/** Fetches the second bit of the NZP representing zero */
bool_t get_nzp_z(cc_t);

/** Fetches the low order bit of the NZP representing positive */
bool_t get_nzp_p(cc_t);

/** Fetches the 5 immediate bits (AND and ADD) from the IR */
imm_5_t get_imm_5(lc3_p);

/** Fetches the 6 PC offset bits (LDR and STR) from the IR */
pc_offset_6_t get_pc_offset_6(lc3_p);

/** Fetches the 9 PC offset bits (BR, LD, LDI, LEA, ST, and STI) from the IR */
pc_offset_9_t get_pc_offset_9(lc3_p);

/** Fetches the 11 PC offset bits (JSR) from the IR */
pc_offset_11_t get_pc_offset_11(lc3_p);

/** Fetches the TRAP vector from the IR */
trap_vector_t get_trap_vector(lc3_p);

/** Zero extends the trap vector to be a full 16-bit word */
word_t zext(trap_vector_t);

/** Allocates and initializes a new LC3 module */
lc3_p lc3_create() {
    lc3_p lc3 = calloc(1, sizeof(lc3_t));
    lc3->cpu = cpu_create();
    lc3->alu = alu_create();
    lc3->memory = memory_create();
    initialize_lc3(lc3);
    return lc3;
}

/** Reinitializes the LC3 module without reallocation */
void lc3_reset(lc3_p lc3) {
    cpu_reset(lc3->cpu);
    alu_reset(lc3->alu);
    memory_reset(lc3->memory);
    initialize_lc3(lc3);
}

/** Deallocates the LC3 module */
void lc3_destroy(lc3_p lc3) {
    cpu_destroy(lc3->cpu);
    alu_destroy(lc3->alu);
    memory_destroy(lc3->memory);
    free(lc3);
}

/** Compiles a snapshot of the LC3 for debugging and/or display purposes. */
const lc3_snapshot_t lc3_get_snapshot(lc3_p lc3) {
    lc3_snapshot_t snapshot;
    snapshot.starting_address = lc3->starting_address;
    snapshot.file_loaded = lc3->is_file_loaded;
    snapshot.is_halted = lc3->is_halted;
    snapshot.cpu_snapshot = cpu_get_snapshot(lc3->cpu);
    snapshot.alu_snapshot = alu_get_snapshot(lc3->alu);
    snapshot.memory_snapshot = memory_get_snapshot(lc3->memory);
    return snapshot;
}

void lc3_fetch(lc3_p lc3) {
    /** Microstate 18 */
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_mar(lc3->cpu, pc);
    cpu_increment_pc(lc3->cpu);

    /** Microstate 33 */
    word_t mar = cpu_get_mar(lc3->cpu);
    word_t data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);

    /** Microstate 35 */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_ir(lc3->cpu, mdr);
}

void lc3_decode(lc3_p lc3) {
    /** Microstate 32
     * The opcode is calculated from the IR and determines the subsequent path of execution */
    lc3->opcode = fetch_opcode(lc3);
}

/** ADD fetch operands */
void lc3_fetch_op_add(lc3_p lc3) {
    /** Microstate 1*/
    if (get_imm_mode(lc3) == TRUE) {
        reg_addr_t sr1 = get_sr1(lc3);
        word_t sr1_data = cpu_get_register(lc3->cpu, sr1);
        imm_5_t imm_5 = get_imm_5(lc3);
        alu_load_sr1(lc3->alu, sr1_data);
        alu_load_sr2(lc3->alu, imm_5);
    } else {
        reg_addr_t sr1 = get_sr1(lc3);
        reg_addr_t sr2 = get_sr2(lc3);
        word_t sr1_data = cpu_get_register(lc3->cpu, sr1);
        word_t sr2_data = cpu_get_register(lc3->cpu, sr2);
        alu_load_sr1(lc3->alu, sr1_data);
        alu_load_sr2(lc3->alu, sr2_data);
    }
}

/** ADD execute */
void lc3_execute_add(lc3_p lc3) {
    /** Microstate 1 */
    alu_add(lc3->alu);
}

/** ADD store */
void lc3_store_add(lc3_p lc3) {
    /** Microstate 1 */
    reg_addr_t dr = get_dr(lc3);
    word_t result = alu_fetch_result(lc3->alu);
    cpu_set_register(lc3->cpu, dr, result);
}

/** AND fetch operands */
void lc3_fetch_op_and(lc3_p lc3) {
    /** Microstate 5 */
    if (get_imm_mode(lc3) == TRUE) {
        reg_addr_t sr1 = get_sr1(lc3);
        word_t sr1_data = cpu_get_register(lc3->cpu, sr1);
        imm_5_t imm_5 = get_imm_5(lc3);
        alu_load_sr1(lc3->alu, sr1_data);
        alu_load_sr2(lc3->alu, imm_5);
    } else {
        reg_addr_t sr1 = get_sr1(lc3);
        reg_addr_t sr2 = get_sr2(lc3);
        word_t sr1_data = cpu_get_register(lc3->cpu, sr1);
        word_t sr2_data = cpu_get_register(lc3->cpu, sr2);
        alu_load_sr1(lc3->alu, sr1_data);
        alu_load_sr2(lc3->alu, sr2_data);
    }
}

/** AND execute */
void lc3_execute_and(lc3_p lc3) {
    /** Microstate 1 */
    alu_and(lc3->alu);
}

/** AND store */
void lc3_store_and(lc3_p lc3) {
    /** Microstate 5 */
    reg_addr_t dr = get_dr(lc3);
    word_t result = alu_fetch_result(lc3->alu);
    cpu_set_register(lc3->cpu, dr, result);
}

/** BR evaluate address */
void lc3_eval_addr_br(lc3_p lc3) {
    /** Microstate 32 */
    bool_t cc_n = cpu_get_cc_n(lc3->cpu);
    bool_t cc_z = cpu_get_cc_z(lc3->cpu);
    bool_t cc_p = cpu_get_cc_p(lc3->cpu);
    cc_t nzp = get_nzp(lc3);
    bool_t nzp_n = get_nzp_n(nzp);
    bool_t nzp_z = get_nzp_z(nzp);
    bool_t nzp_p = get_nzp_p(nzp);
    lc3->branch_enabled = (nzp_n & cc_n) | (nzp_z & cc_z) | (nzp_p & cc_p);

    if (lc3->branch_enabled) {
        /** Microstate 22 */
        pc_offset_9_t offset = get_pc_offset_9(lc3);
        word_t pc = cpu_get_pc(lc3->cpu);
        lc3->eval_addr_calculation = pc + offset;
    }
}

/** BR execute */
void lc3_execute_br(lc3_p lc3) {
    if (lc3->branch_enabled) {
        /** Microstate 22 */
        cpu_set_pc(lc3->cpu, lc3->eval_addr_calculation);
    }
}

/** JMP evaluate address */
void lc3_eval_addr_jmp(lc3_p lc3) {
    /** Microstate 12 */
    reg_addr_t base_r = get_sr1(lc3); /* Actually "Base Register" */
    word_t base_addr = cpu_get_register(lc3->cpu, base_r);
    lc3->eval_addr_calculation = base_addr;
}

/** JMP store */
void lc3_store_jmp(lc3_p lc3) {
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_register(lc3->cpu, R7, pc);
    cpu_set_pc(lc3->cpu, lc3->eval_addr_calculation);
}

/** JSR evaulate address */
void lc3_eval_addr_jsr(lc3_p lc3) {
    /** Microstate 4 */
    if (get_jsr_imm_mode(lc3) == TRUE) {
        /** Microstate 21 */
        pc_offset_11_t offset = get_pc_offset_11(lc3);
        word_t pc = cpu_get_pc(lc3->cpu);
        lc3->eval_addr_calculation = pc + offset;
    } else {
        reg_addr_t base_r = get_sr1(lc3); /* Actually "Base Register" */
        word_t base_addr = cpu_get_register(lc3->cpu, base_r);
        lc3->eval_addr_calculation = base_addr;
    }
}

/** JSR store */
void lc3_store_jsr(lc3_p lc3) {
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_register(lc3->cpu, R7, pc);
    cpu_set_pc(lc3->cpu, lc3->eval_addr_calculation);
}

/** LD evaluate address */
void lc3_eval_addr_ld(lc3_p lc3) {
    /** Microstate 2 */
    pc_offset_9_t offset = get_pc_offset_9(lc3);
    word_t pc = cpu_get_pc(lc3->cpu);
    lc3->eval_addr_calculation = pc + offset;
}

/** LD fetch operands */
void lc3_fetch_op_ld(lc3_p lc3) {
    /** Microstate 2 */
    cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);

    /** Microstate 25 */
    word_t mar = cpu_get_mar(lc3->cpu);
    word_t data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);
}

/** LD store */
void lc3_store_ld(lc3_p lc3) {
    /** Microstate 27 */
    reg_addr_t dr = get_dr(lc3);
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_register(lc3->cpu, dr, mdr);
}

/** LDI evaluate address */
void lc3_eval_addr_ldi(lc3_p lc3) {
    /** Microstate 10 */
    pc_offset_9_t offset = get_pc_offset_9(lc3);
    word_t pc = cpu_get_pc(lc3->cpu);
    lc3->eval_addr_calculation = pc + offset;
}

/** LDI fetch operands */
void lc3_fetch_op_ldi(lc3_p lc3) {
    /** Microstate 10 */
    cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);

    /** Microstate 21 */
    word_t mar = cpu_get_mar(lc3->cpu);
    word_t data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);

    /** Microstate 26 */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_mar(lc3->cpu, mdr);

    /** Microstate 25 */
    mar = cpu_get_mar(lc3->cpu);
    data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);
}

/** LDI store */
void lc3_store_ldi(lc3_p lc3) {
    /** Microstate 27 */
    reg_addr_t dr = get_dr(lc3);
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_register(lc3->cpu, dr, mdr);
}

/** LDR evaluate address */
void lc3_eval_addr_ldr(lc3_p lc3) {
    /** Microstate 6 */
    reg_addr_t base_r = get_sr1(lc3); /* Actually Base Register */
    word_t base_addr = cpu_get_register(lc3->cpu, base_r);
    pc_offset_6_t offset = get_pc_offset_6(lc3);
    lc3->eval_addr_calculation = base_addr + offset;
}

/** LDR fetch operands */
void lc3_fetch_op_ldr(lc3_p lc3) {
    /** Microstate 6 */
    cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);

    /** Microstate 25 */
    word_t mar = cpu_get_mar(lc3->cpu);
    word_t data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);
}

/** LDR store */
void lc3_store_ldr(lc3_p lc3) {
    /** Microstate 27 */
    reg_addr_t dr = get_dr(lc3);
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_register(lc3->cpu, dr, mdr);
}

/** LEA evaluate address */
void lc3_eval_addr_lea(lc3_p lc3) {
    /** Microstate 14 */
    pc_offset_9_t offset = get_pc_offset_9(lc3);
    word_t pc = cpu_get_pc(lc3->cpu);
    lc3->eval_addr_calculation = pc + offset;
}

/** LEA store */
void lc3_store_lea(lc3_p lc3) {
    /** Microstate 14 */
    reg_addr_t dr = get_dr(lc3);
    cpu_set_register(lc3->cpu, dr, lc3->eval_addr_calculation);
}

/** NOT fetch operands */
void lc3_fetch_op_not(lc3_p lc3) {
    /** Microstate 9 */
    reg_addr_t sr1 = get_sr1(lc3);
    word_t sr1_data = cpu_get_register(lc3->cpu, sr1);
    alu_load_sr1(lc3->alu, sr1_data);
}

/** NOT execute */
void lc3_execute_not(lc3_p lc3) {
    /** Microstate 1 */
    alu_not(lc3->alu);
}

/** NOT store */
void lc3_store_not(lc3_p lc3) {
    /** Microstate 9 */
    reg_addr_t dr = get_dr(lc3);
    word_t result = alu_fetch_result(lc3->alu);
    cpu_set_register(lc3->cpu, dr, result);
}

/** ST evaluate address */
void lc3_eval_addr_st(lc3_p lc3) {
    /** Microstate 3 */
    pc_offset_9_t offset = get_pc_offset_9(lc3);
    word_t pc = cpu_get_pc(lc3->cpu);
    lc3->eval_addr_calculation = pc + offset;
}

/** ST fetch operands */
void lc3_fetch_op_st(lc3_p lc3) {
    /** Microstate 3 */
    cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);

    /** Microstate 23 */
    reg_addr_t sr = get_dr(lc3); /* Actually "Source Register" */
    word_t sr_data = cpu_get_register(lc3->cpu, sr);
    cpu_set_mdr(lc3->cpu, sr_data);
}

/** ST store */
void lc3_store_st(lc3_p lc3) {
    /** Microstate 16 */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    word_t mar = cpu_get_mar(lc3->cpu);
    memory_write(lc3->memory, mar, mdr);
}

/** STI evaluate address */
void lc3_eval_addr_sti(lc3_p lc3) {
    /** Microstate 11 */
    pc_offset_9_t offset = get_pc_offset_9(lc3);
    word_t pc = cpu_get_pc(lc3->cpu);
    lc3->eval_addr_calculation = pc + offset;
}

/** STI fetch operands */
void lc3_fetch_op_sti(lc3_p lc3) {
    /** Microstate 11 */
    cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);

    /** Microstate 29 */
    word_t mar = cpu_get_mar(lc3->cpu);
    word_t data = memory_get_data(lc3->memory, mar);
    cpu_set_mdr(lc3->cpu, data);

    /** Microstate 31 */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    cpu_set_mar(lc3->cpu, mdr);

    /** Microstate 23 */
    reg_addr_t sr = get_dr(lc3); /* Actually "Source Register" */
    word_t sr_data = cpu_get_register(lc3->cpu, sr);
    cpu_set_mdr(lc3->cpu, sr_data);
}

/** STI store */
void lc3_store_sti(lc3_p lc3) {
    /** Microstate 16 */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    word_t mar = cpu_get_mar(lc3->cpu);
    memory_write(lc3->memory, mar, mdr);
}

/** STR evalate address */
void lc3_eval_addr_str(lc3_p lc3) {
    /** Microstate 7 */
    reg_addr_t base_r = get_sr1(lc3); /* Actually "Base Register" */
    word_t base_addr = cpu_get_register(lc3->cpu, base_r);
    pc_offset_6_t offset = get_pc_offset_6(lc3);
    lc3->eval_addr_calculation = base_addr + offset;
}

/** STR fetch operands */
void lc3_fetch_op_str(lc3_p lc3) {
    /** Microstate 7 */
    cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);

    /** Microstate 23 */
    reg_addr_t sr = get_dr(lc3); /* Actually "Source Register" */
    word_t sr_data = cpu_get_register(lc3->cpu, sr);
    cpu_set_mdr(lc3->cpu, sr_data);
}

/** STR store */
void lc3_store_str(lc3_p lc3) {
    /** Microstate 16 */
    word_t mdr = cpu_get_mdr(lc3->cpu);
    word_t mar = cpu_get_mar(lc3->cpu);
    memory_write(lc3->memory, mar, mdr);
}

/** TRAP fetch operands */
void lc3_fetch_op_trap(lc3_p lc3) {
    /** Microstate 15 */
    trap_vector_t vector = get_trap_vector(lc3);
    cpu_set_mar(lc3->cpu, zext(vector));
}

/** Stack fetch operands */
void lc3_fetch_op_stack(lc3_p lc3) {
    /** cpu_set_mar(lc3->cpu, lc3->eval_addr_calculation);
     * Currently the stack doesn't have an evaluate address calculation. */

    /** R6 contains the stack pointer. This will be used for overflow/underflow checks */
    word_t r6_data = cpu_get_register(lc3->cpu, R6);

    /** Push/ST */
    if (get_imm_mode(lc3) == STACK_PUSH) {
        /** check for overflow here (if fail, R5 = 0 and break; else R5 = 1) */
        if (r6_data < STACK_MAX) {
            cpu_set_register(lc3->cpu, R5, STACK_ERROR);
            return;
        } else {
            cpu_set_register(lc3->cpu, R5, STACK_SUCCESS);
        }

        reg_addr_t sr = get_dr(lc3);
        word_t sr_data = cpu_get_register(lc3->cpu, sr);
        cpu_set_mdr(lc3->cpu, sr_data); /* MDR now contains contents to be pushed */
        r6_data--;                      /* Decrement and store the stack pointer */
        cpu_set_mar(lc3->cpu, r6_data); /* MAR <- R6 */
        cpu_set_register(lc3->cpu, R6, r6_data);

    /** Pop/LD */
    } else {
        /** check for underflow here (if fail, R5 = 0 and break; else R5 = 1) */
        if (r6_data > STACK_LAST) {
            cpu_set_register(lc3->cpu, R5, STACK_ERROR);
            return;
        } else {
            cpu_set_register(lc3->cpu, R5, STACK_SUCCESS);
        }

        cpu_set_mar(lc3->cpu, r6_data); /* MAR <- R6 */
        word_t mar = cpu_get_mar(lc3->cpu);
        word_t data = memory_get_data(lc3->memory, mar);
        cpu_set_mdr(lc3->cpu, data);    /* MDR now contains contents to be popped */
        r6_data++;                      /* Increment and store the stack pointer */
        cpu_set_register(lc3->cpu, R6, r6_data);
    }
}

/** Stack store */
void lc3_store_stack(lc3_p lc3) {
    word_t r5_data = cpu_get_register(lc3->cpu, R5);
    if (r5_data == STACK_ERROR) {
        return;
    }

    /** Push/ST */
    if (get_imm_mode(lc3) == STACK_PUSH) {
        word_t mdr = cpu_get_mdr(lc3->cpu);
        word_t mar = cpu_get_mar(lc3->cpu);
        memory_write(lc3->memory, mar, mdr); /* push complete */

    /** Pop/LD */
    } else {
        reg_addr_t dr = get_dr(lc3);
        word_t mdr = cpu_get_mdr(lc3->cpu);
        cpu_set_register(lc3->cpu, dr, mdr); /* pop complete */
    }
}

/** TRAP execute */
word_t lc3_execute_trap(lc3_p lc3) {
    /** This is an extra credit opportunity to fully implement trap according to Microstates
     * 15, 28, 30 */
    return cpu_get_mar(lc3->cpu);
}

/** LC3 portion of the GETC routine */
void lc3_trap_x20(lc3_p lc3, char c) {
    /** Even though this is not really a service routine, we'll simulate the R7 behavior */
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_register(lc3->cpu, R7, pc);

    cpu_set_register(lc3->cpu, R0, (word_t)c);

    word_t r7_data = cpu_get_register(lc3->cpu, R7);
    cpu_set_pc(lc3->cpu, r7_data);
}

/** LC3 portion of the OUT routine */
char lc3_trap_x21(lc3_p lc3) {
    /** Even though this is not really a service routine, we'll simulate the R7 behavior */
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_register(lc3->cpu, R7, pc);

    word_t data = cpu_get_register(lc3->cpu, R0);

    word_t r7_data = cpu_get_register(lc3->cpu, R7);
    cpu_set_pc(lc3->cpu, r7_data);
    return (char)data;
}

/** LC3 portion of the PUTS routine */
char lc3_trap_x22(lc3_p lc3) {
    /** Even though this is not really a service routine, we'll simulate the R7 behavior */
    word_t pc = cpu_get_pc(lc3->cpu);
    cpu_set_register(lc3->cpu, R7, pc);

    word_t char_ptr = cpu_get_register(lc3->cpu, R0);
    cpu_set_register(lc3->cpu, R0, char_ptr + 1);
    word_t data = memory_get_data(lc3->memory, char_ptr);

    word_t r7_data = cpu_get_register(lc3->cpu, R7);
    cpu_set_pc(lc3->cpu, r7_data);
    return (char)data;
}

/** LC3 HALT routine */
void lc3_trap_x25(lc3_p lc3) {
    /** Even though this is not really a service routine, we'll simulate the R7 behavior */
    word_t pc = cpu_get_pc(lc3->cpu);

    cpu_set_register(lc3->cpu, R7, pc);
    if (lc3_is_halted(lc3) == FALSE) {
        lc3_toggle_halted(lc3);
    }
}

/** Gets the starting address for the PC according to the first line in the loaded hex file */
word_t lc3_get_starting_address(lc3_p lc3) { return lc3->starting_address; }

/** Sets the starting address for the PC according to the first line in the loaded hex file */
void lc3_set_starting_address(lc3_p lc3, word_t starting_address) {
    lc3->starting_address = starting_address;
    cpu_set_pc(lc3->cpu, starting_address);
}

/** Checks whether the LC3 is halted */
bool_t lc3_is_halted(lc3_p lc3) { return lc3->is_halted; }

/** Toggles whether the LC3 is halted */
void lc3_toggle_halted(lc3_p lc3) { lc3->is_halted = !lc3->is_halted; }

/** Checks whether the LC3 has a file loaded */
bool_t lc3_has_file_loaded(lc3_p lc3) { return lc3->is_file_loaded; }

/** Toggles whether the LC3 has a file loaded */
void lc3_toggle_file_loaded(lc3_p lc3) { lc3->is_file_loaded = !(lc3->is_file_loaded); }

/** Gets the current microstate of the LC3 */
state_t lc3_get_state(lc3_p lc3) { return lc3->state; }

/** Sets the current microstate of the LC3 */
void lc3_set_state(lc3_p lc3, state_t state) { lc3->state = state; }

/** Gets the opcode stored in the LC3 struct */
opcode_t lc3_get_opcode(lc3_p lc3) { return lc3->opcode; }

/** Gets the PC from the CPU. Used for checking against breakpoints in the main simulator loop
 */
word_t lc3_get_pc(lc3_p lc3) { return cpu_get_pc(lc3->cpu); }

/** Sets memory data at the specified address. This is used for loading a file to memory and
 * editing memory from the Display */
void lc3_set_memory(lc3_p lc3, word_t address, word_t data) {
    memory_write(lc3->memory, address, data);
}

/** Sets LC3 values to default starting values */
void initialize_lc3(lc3_p lc3) {
    lc3->starting_address = MEMORY_ADDRESS_MIN;
    lc3->is_halted = FALSE;
    lc3->is_file_loaded = FALSE;
    initialize_intrastate(lc3);
    cpu_set_register(lc3->cpu, R6, STACK_BASE);
}

/** Reinitializes the variables used during each phase of instruction processing */
void initialize_intrastate(lc3_p lc3) {
    lc3->state = STATE_FETCH;
    lc3->opcode = OPCODE_TRAP;
    lc3->eval_addr_calculation = 0;
    lc3->trap_vector = 0;
}

/** Fetches the opcode from the IR */
opcode_t fetch_opcode(lc3_p lc3) {
    /** No masking needed */
    opcode_t opcode = cpu_get_ir(lc3->cpu) >> BITSHIFT_OPCODE;
    return opcode;
}

/** Fetches the destination register from the IR */
reg_addr_t get_dr(lc3_p lc3) {
    word_t masked_ir = cpu_get_ir(lc3->cpu) & MASK_DR;
    return (reg_addr_t)(masked_ir >> BITSHIFT_DR);
}

/** Fetches the source register 1 from the IR */
reg_addr_t get_sr1(lc3_p lc3) {
    word_t masked_ir = cpu_get_ir(lc3->cpu) & MASK_SR1;
    return (reg_addr_t)(masked_ir >> BITSHIFT_SR1);
}

/** Fetches the source register 2 from the IR */
reg_addr_t get_sr2(lc3_p lc3) {
    word_t masked_ir = cpu_get_ir(lc3->cpu) & MASK_SR2;
    return masked_ir; /* No shift needed */
}

/** Fetches bit 5 (immediate mode flag for AND and ADD) from the IR */
bool_t get_imm_mode(lc3_p lc3) {
    word_t masked_ir = cpu_get_ir(lc3->cpu) & MASK_BIT5;
    return (bool_t)(masked_ir >> BITSHIFT_BIT5);
}

/** Fetches the immediate bits from the IR */
bool_t get_jsr_imm_mode(lc3_p lc3) {
    word_t masked_ir = cpu_get_ir(lc3->cpu) & MASK_BIT11;
    return (bool_t)(masked_ir >> BITSHIFT_BIT11);
}

/** Fetches the NZP bits from the IR */
cc_t get_nzp(lc3_p lc3) {
    word_t masked_ir = cpu_get_ir(lc3->cpu) & MASK_NZP;
    return (cc_t)(masked_ir >> BITSHIFT_NZP);
}

/** Fetches the high order bit of the NZP representing negative */
bool_t get_nzp_n(cc_t nzp) {
    /** No masking needed */
    return (bool_t)(nzp >> BITSHIFT_CC_N);
}

/** Fetches the second bit of the NZP representing zero */
bool_t get_nzp_z(cc_t nzp) {
    /** Mask and shift */
    return (bool_t)((nzp & MASK_CC_Z) >> BITSHIFT_CC_Z);
}

/** Fetches the low order bit of the NZP representing positive */
bool_t get_nzp_p(cc_t nzp) {
    /** No shifting needed */
    return (bool_t)(nzp & MASK_CC_P);
}

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
trap_vector_t get_trap_vector(lc3_p lc3) { return cpu_get_ir(lc3->cpu) & MASK_TRAPVECT8; }

/** Zero extends the trap vector to be a full 16-bit word */
word_t zext(trap_vector_t trap_vector) { return (word_t)trap_vector; }
