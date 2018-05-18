/* LC-3 Emulator
 *
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the
 * 16-bit LC-3 machine based a finite state machine (FSM) interpretation of its
 * operations.
 */

#include "slc3.h"
#include "lc3.h"
#include "display_monitor.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

FILE *open_file();

/*
 * Main method for the LC-3 Emulator.
 *
 * The command line argument passed in is what will be populated into
 * the LC-3's memory at runtime. For example, if the program is run with
 * input "0x1694", that hexadecimal value will be stored into the instruction
 * register (IR) and can be thought of in binary as 0001 0110 1001 0100, which
 * (based on the four highest-order bits) is an ADD instruction for the LC-3
 * (from its instruction set).
 */
int main(int argc, char *argv[]) {
    /* Creating and initializing a CPU struct. */
    lc3_p lc3 = (lc3_p)calloc(1, sizeof(struct lc3_t));
    if (!lc3)
        return 1;
    lc3_init(lc3);

    // If there is an argument, attempt to used the first as a file name.
    char *fileName = argv[1]; // char *fileName = "./hex/HW3.hex";
    if (fileName != NULL) {
        load_file_to_memory(cpu, open_file2(fileName));
    }

    /* Initialize and run the LC-3 from the debug monitor */
    display_monitor_init(cpu);

    int monitor_return = display_monitor_loop(cpu);

    /* If the user has selected MONITOR_STEP, then lets continue executing the
     * LC-3 */
    while (monitor_return != MONITOR_QUIT) {
        switch (monitor_return) {
        case MONITOR_UPDATE:
            /* No operations this loop, just do the display monitor loop again */
            break;

        case MONITOR_LOAD:
            load_file_to_memory(cpu, open_file());
            break;

        case MONITOR_STEP:
            if (!is_halted) {
                controller(cpu);
            }
            break;
        case MONITOR_RUN:
            if (!is_halted) {
                do {
                    controller(cpu);
                } while (!is_halted && !has_breakpoint[cpu->pc]);
            }
        }
        monitor_return = display_monitor_loop(cpu);
    }

    /* Memory cleanup. */
    display_monitor_destroy();
    free(cpu);

    return 0;
}

/*
 * The controller method of the LC-3. This contains much of the complete
 * instruction cycle of the LC-3.
 */
void controller(lc3_p lc3) {

    opcode_t opcode;
    reg_addr_t dr, sr1, sr2;

    bool_t is_cycle_complete = FALSE;

    /* Ensuring that CPU pointer being passed into the controller is valid. */
    if (!lc3) {
        exit(1);
    }

    /* Creating integers array to store a binary representation of
           the contents of the CPU's instruction register. */
    // int *binary_IR_contents = (int *)malloc(sizeof(int) * NUM_OF_BITS);
    // int *binary_IR_helper_array = (int *)malloc(sizeof(int) * NUM_OF_BITS);

    /* Beginning instruction cycle. */
    set_state(FETCH);
    while (!is_cycle_complete) {
        switch (lc3_get_state(lc3)) {
        /* The first state of the instruction cycle, the "fetch" state. */
        case FETCH:
            lc3_ms_18(lc3);
            lc3_ms_33(lc3);
            break;

        /* The second state of the instruction cycle, the "decode" state. */
        case DECODE:
            /* Corresponding to FSM state 32. Most of these decoding functions are
                                   delegated to helper functions. */
            opcode = (cpu->ir & MASK_OPCODE) >>
                     BITSHIFT_OPCODE; // Input is the four-bit opcode IR[15:12].
                                      // The output line asserted is the one
                                      // corresponding to the opcode at the input.
            state = EVAL_ADDR;        // Moving to next state.
            break;

        /* The third state of the instruction cycle, the "evaluate address" state.
         */
        case EVAL_ADDR:
            switch (opcode) {
            case LD:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR;
                offset = cpu->ir & MASK_PCOFFSET9;
                offset = SEXT(offset, BIT_PCOFFSET9);
                cpu->mar = cpu->pc + offset; // microstate 2.
                cpu->mdr = memory[cpu->mar]; // microstate 25.
                break;
            case LDR:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR;
                sr1 = (cpu->ir & MASK_SR1) >> BITSHIFT_SR1;
                offset = cpu->ir & MASK_PCOFFSET6;
                offset = SEXT(offset, BIT_PCOFFSET6);
                cpu->mar = cpu->registers[sr1] + offset;
                cpu->mdr = memory[cpu->mar];
                break;
            case ST:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR; // This is actually a source
                                                         // register, but still use dr.
                offset = cpu->ir & MASK_PCOFFSET9;
                offset = SEXT(offset, BIT_PCOFFSET9);
                cpu->mar = cpu->pc + offset; // microstate 2.
                break;
            case STR:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR;    // actually source register
                sr1 = (cpu->ir & MASK_SR1) >> BITSHIFT_SR1; // base register
                offset = cpu->ir & MASK_PCOFFSET6;
                offset = SEXT(offset, BIT_PCOFFSET6);
                cpu->mar = cpu->registers[sr1] + offset;
                break;
            case LEA:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR;
                offset = cpu->ir & MASK_PCOFFSET9;
                offset = SEXT(offset, BIT_PCOFFSET9);
                break;
            case JSR:
                // TODO use a switch block for the bit to distinguish JSR and JSRR.
                offset = cpu->ir & MASK_PCOFFSET11;
                offset = SEXT(offset, BIT_PCOFFSET11);
                break;
            }
            state = FETCH_OP; // Moving to next state.
            break;

        /* The fourth state of the instruction cycle, the "fetch operands" state. */
        case FETCH_OP:
            switch (opcode) {
            // get operands out of registers into A, B of ALU
            // or get memory for load instr.
            case ADD:
            case AND:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR;
                sr1 = (cpu->ir & MASK_SR1) >> BITSHIFT_SR1;
                imm_mode = (cpu->ir & MASK_BIT5) >> BITSHIFT_BIT5;
                if (imm_mode == FALSE) {
                    sr2 = cpu->ir & MASK_SR2; // no shift needed.
                } else if (imm_mode == TRUE) {
                    immed = cpu->ir & MASK_IMMED5; // no shift needed.
                    immed = SEXT(immed, BIT_IMMED);
                }
                // The book page 106 says current microprocessors can be done
                // simultaneously during fetch, but this simulator is old skool.
                break;
            case NOT:
                dr = (cpu->ir & MASK_DR) >> BITSHIFT_DR;
                sr1 = (cpu->ir & MASK_SR1) >> BITSHIFT_SR1;
                break;
            case TRAP:
                vector = cpu->ir & MASK_TRAPVECT8; // No shift needed.
                break;
            case ST: // Same as LD.
            case STR:
                // Book page 124.
                cpu->mdr = cpu->registers[dr];
                break;
            case JMP:
                sr1 = (cpu->ir & MASK_SR1) >> BITSHIFT_SR1;
                break;
            case BR:
                nzp = (cpu->ir & MASK_NZP) >> BITSHIFT_CC;
                offset = cpu->ir & MASK_PCOFFSET9;
                break;
            case JSR:
                imm_mode = (cpu->ir & MASK_BIT11) >> BITSHIFT_BIT11;
                cpu->registers[7] = cpu->pc;
                if (bit11 == 0) { // JSRR
                    sr1 = (cpu->ir & MASK_SR1) >> BITSHIFT_SR1;
                    cpu->pc = cpu->registers[sr1];
                } else { // JSR
                    cpu->pc += offset;
                }
                break;
            }
            state = EXECUTE; // Moving to next state.
            break;

        /* The fifth state of the instruction cycle, the "execute" state. */
        case EXECUTE:
            switch (opcode) {
            case ADD:
                if (imm_mode == FALSE) {
                    cpu->mdr = cpu->registers[sr2] + cpu->registers[sr1];
                } else if (imm_mode == TRUE) {
                    cpu->mdr = cpu->registers[sr1] + immed;
                }
                setCC(cpu->mdr, cpu); // TODO should this be set in this phase?
                break;
            case AND:
                if (imm_mode == 0) {
                    cpu->mdr = cpu->registers[sr2] & cpu->registers[sr1];
                } else if (imm_mode == 1) {
                    cpu->mdr = cpu->registers[sr1] & immed;
                }
                setCC(cpu->mdr, cpu); // TODO should this be set in this phase?
                break;
            case NOT:
                cpu->mdr = ~cpu->registers[sr1]; // Interpret as a negative if the
                                                 // leading bit is a 1.
                setCC(cpu->mdr, cpu);            // TODO should this be set in this phase?
                break;
            case TRAP:
                // Book page 222.
                cpu->registers[7] = cpu->pc; // Store the PC in R7 before loading PC with the
                                             // starting address of the service routine.
                trap(vector, cpu);
                break;
            case JMP:
                cpu->pc = cpu->registers[sr1];
                break;
            case BR:
                offset = SEXT(offset, BIT_PCOFFSET9);
                if (branchEnabled(nzp, cpu)) {
                    cpu->pc += (offset);
                }
                break;
            }
            state = STORE; // Moving to next state.
            break;

        /* The sixth state of the instruction cycle, the "store" state. */
        case STORE:
            /* Corresponding to FSM state 16. */
            switch (opcode) {
            // write back to register or store MDR into memory
            case ADD:
            case AND: // Same as ADD
            case NOT: // Same as AND and AND.
                cpu->registers[dr] = cpu->mdr;
                break;
            case LD:
            case LDR:
                cpu->registers[dr] = cpu->mdr; // Load into the register.
                setCC(cpu->registers[dr], cpu);
                break;
            case ST:
            case STR:
                memory[cpu->mar] = cpu->mdr; // Store into memory.
                break;
            case LEA:
                cpu->registers[dr] = cpu->pc + offset;
                setCC(cpu->registers[dr], cpu);
                break;
                // do any clean up here in prep for the next complete cycle
                isCycleComplete = true;
                state = FETCH;
                break;
            }
            isCycleComplete = true;
            break;
        } // end switch (state)

    } // end while (isCycleComplete)

    if (is_halted) {
        cpu->pc = 0;
    }
} // end controller()

/*
 * This function that determines and executes the appropriate trap routine based
 * on the trap vector passed.
 */
void trap(unsigned short vector, CPU_p cpu) {
    char c;

    switch (vector) {
    case TRAP_VECTOR_X25:
        /* Exit program state. */
        cpu->state = FETCH;
        is_halted = 1;
        break;

    case TRAP_VECTOR_X20:
        /* getch */
        c = display_monitor_get_input();
        cpu->registers[R0] = c;
        break;

    case TRAP_VECTOR_X21:
        display_monitor_print_output(cpu->registers[R0]);
        break;

    case TRAP_VECTOR_X22:
        while (memory[cpu->registers[R0]] != '\0') {
            display_monitor_print_output(memory[cpu->registers[R0]]);
            cpu->registers[R0]++;
        }
        break;
    }
}

/**
 * This function will take the loaction of the high order bit of the immediate
 * value and sign extend it so that if the high order bit is a 1, then it will
 * be converted to negative value.
 *
 * @param value is the number to be sign extended.
 * @param instance determines what the high order bit is of the value.
 */
short SEXT(unsigned short theValue, int highOrderBit) {
    short value = (short)theValue;
    switch (highOrderBit) {
    case BIT_IMMED:
        if (((value & BIT_IMMED) >> BITSHIFT_NEGATIVE_IMMEDIATE) == 1)
            value = value | MASK_NEGATIVE_IMMEDIATE;
        break;
    case BIT_PCOFFSET11:
        if (((value & BIT_PCOFFSET11) >> BITSHIFT_NEGATIVE_PCOFFSET11) == 1)
            value = value | MASK_NEGATIVE_PCOFFSET11;
        break;
    case BIT_PCOFFSET9:
        if (((value & BIT_PCOFFSET9) >> BITSHIFT_NEGATIVE_PCOFFSET9) == 1)
            value = value | MASK_NEGATIVE_PCOFFSET9;
        break;
    case BIT_PCOFFSET6:
        if (((value & BIT_PCOFFSET6) >> BITSHIFT_NEGATIVE_PCOFFSET6) == 1)
            value = value | MASK_NEGATIVE_PCOFFSET6;
        break;
    }
    return value;
}

/**
 * This function will determine the condition code based on the value passed
 */
void setCC(unsigned short value, CPU_p cpu) {
    short signedValue = value;
    if (signedValue < 0) {
        cpu->ccN = 1;
        cpu->ccZ = 0;
        cpu->ccP = 0;
    } else if (signedValue == 0) {
        cpu->ccN = 0;
        cpu->ccZ = 1;
        cpu->ccP = 0;
    } else {
        cpu->ccN = 0;
        cpu->ccZ = 0;
        cpu->ccP = 1;
    }
}

/**
 * Sets the condition code resulting by the resulting computer value.
 * @param value the value that was recently computed.
 * @return the condition code that represents the 3bit NZP as binary.
 */
bool branchEnabled(unsigned short nzp, CPU_p cpu) {
    bool result = false;
    switch (nzp) {
    case CONDITION_NZP:
        result = true;
        break;
    case CONDITION_NP:
        if (cpu->ccN || cpu->ccP)
            result = true;
        break;
    case CONDITION_NZ:
        if (cpu->ccN || cpu->ccZ)
            result = true;
        break;
    case CONDITION_ZP:
        if (cpu->ccZ || cpu->ccP)
            result = true;
        break;
    case CONDITION_N:
        if (cpu->ccN)
            result = true;
        break;
    case CONDITION_Z:
        if (cpu->ccZ)
            result = true;
        break;
    case CONDITION_P:
        if (cpu->ccP)
            result = true;
        break;
    }
    return result;
}

/*
 * This function will set the condition code based on the input number passed.
 */
void set_condition_code(int input_number, CPU_p cpu) {
    if (input_number > 0) {
        cpu->ccN = 0;
        cpu->ccZ = 0;
        cpu->ccP = 1; /* P */
    } else if (input_number < 0) {
        cpu->ccN = 1; /* N */
        cpu->ccZ = 0;
        cpu->ccP = 0;
    } else {
        cpu->ccN = 0;
        cpu->ccZ = 1; /* Z */
        cpu->ccP = 0;
    }
}

/*
 * This function determines and returns a 1 or 0 to represent if a branch
 * should be taken or not.
 */
int get_branch_enabled(int *binary_IR_contents, int *binary_IR_helper_array, CPU_p cpu) {
    /* Corresponds to bits [4:6] in memory. */
    binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 4, 3);

    /* Convert the condition code to an istarting_addressnteger array for bit comparison. */
    int *condition_code = (int *)malloc(sizeof(int) * 3);
    int i;
    int temp_array[3];

    temp_array[0] = cpu->ccN;
    temp_array[1] = cpu->ccZ;
    temp_array[2] = cpu->ccP;

    for (i = 0; i < 3; i++) {
        condition_code[2 - i] = temp_array[i];
    }

    /* Finally, determine if branch should be enabled. */
    int result = 0;
    for (i = 0; i < 3; i++) {
        if (binary_IR_helper_array[i] == condition_code[i]) {
            result = 1; /* If any bit is shared between the two, set branch enabled
                           to 1. */
        }
    }

    /* Memory cleanup. */
    free(condition_code);
    return result;
}

/*
 * This function prints a binary representation of a given unsigned integer
 * value to the screen.
 */
void print_binary_form(unsigned value) {
    if (value > 1)
        print_binary_form(value / 2);

    printf("%d", value % 2);
}

/*
 * This function will allow the opening of files
 */
FILE *open_file() {
    /* Attempt to open file. If file isn't found or otherwise null, allow user to
       press enter to return to main program of the menu. */
    FILE *input_file_pointer;
    input_file_pointer = fopen(load_file_input, "r");
    if (input_file_pointer == NULL) {
        /*
         * Error checking is hard to do with this file structure... How can we get
         * an error message back to the display_monitor? We would want an error
         * message to appear right under the user selection options.
         */
    }
    return input_file_pointer;
}

/*
 * This function will allow the opening of files
 */
FILE *open_file2(char *theFileName) {
    /* Attempt to open file. If file isn't found or otherwise null, allow user to
    press enter to return to main program of the menu. */
    FILE *input_file_pointer;
    input_file_pointer = fopen(theFileName, "r");
    if (input_file_pointer == NULL) {
        // TODO Should display a message and re-prompt the user.
    }
    return input_file_pointer;
}

/*
 * This function allows for the loading of hex files into memory.
 */
void load_file_to_memory(CPU_p cpu, FILE *input_file_pointer) {
    char line[8];
    fgets(line, sizeof(line), input_file_pointer);

    /*
     * Subtract 0x3000 from first hex value in file to be starting memory location
     * Note: This requires the first line of the hex file to not be less than
     * x3000 since this is an unsigned short.
     *
     * IDEA: Create offset variable to hold difference between given start
     * location and x3000? This could be positive or negative value? Then compute
     * from this value.
     */
    unsigned short first_address = strtol(line, NULL, 16);
    starting_address = first_address;

    /* Read through file line by line and store to CPU memory. */
    unsigned short address;
    int result = 1;
    int i = 0;
    while (fscanf(input_file_pointer, "%hx", &address) != EOF) {
        memory[i] = address;
        i += 1;
    }
    file_loaded = 1;
}
