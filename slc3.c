/* LC-3 Emulator
 *
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the
 * 16-bit LC-3 machine based a finite state machine (FSM) interpretation of its
 * operations.
 */


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "slc3.h"
#include "display.h"
#include "lc3.h"

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
    /* Creating and initializing a LC3 object. */
    lc3_p lc3 = lc3_create();

    // If there is an argument, attempt to used the first as a file name.
    char *fileName = argv[1]; // char *fileName = "./hex/HW3.hex";
    if (fileName != NULL) {
        load_file_to_memory(cpu, open_file2(fileName));
    }

    display_p disp = display_create();

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
            if (lc3_is_halted(lc3) == FALSE) {
                controller(cpu);
            }
            break;
        case MONITOR_RUN:
            if (lc3_is_halted(lc3) == FALSE) {
                do {
                    controller(cpu);
                } while (lc3_is_halted(lc3) == FALSE && !has_breakpoint[cpu->pc]);
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
    /** This is set to true at the end of the STORE phase and allows execution control to be
     * passed back to the main loop */
    bool_t is_cycle_complete = FALSE;

    /* Ensuring that CPU pointer being passed into the controller is valid. */
    if (!lc3) {
        exit(1);
    }

    /* Beginning instruction cycle. */
    set_state(STATE_FETCH);
    while (!is_cycle_complete) {
        switch (lc3_get_state(lc3)) {
        /* The first state of the instruction cycle, the "fetch" state. */
        case STATE_FETCH:
            lc3_fetch(lc3);
            lc3_set_state(lc3, STATE_DECODE);
            break;

        /* The second state of the instruction cycle, the "decode" state. */
        case STATE_DECODE:
            /* Corresponding to FSM state 32. */
            lc3_decode(lc3);
            lc3_set_state(lc3, STATE_EVAL_ADDR);
            break;

        /* The third state of the instruction cycle, the "evaluate address" state.
         */
        case STATE_EVAL_ADDR:
            switch (lc3_get_opcode(lc3)) {
            case OPCODE_LD:
                lc3_eval_addr_ld(lc3);
                break;
            case OPCODE_LDI:
                lc3_eval_addr_ldi(lc3);
                break;
            case OPCODE_LDR:
                lc3_eval_addr_ldr(lc3);
                break;
            case OPCODE_ST:
                lc3_eval_addr_st(lc3);
                break;
            case OPCODE_STI:
                lc3_eval_addr_sti(lc3);
                break;
            case OPCODE_STR:
                lc3_eval_addr_str(lc3);
                break;
            case OPCODE_JMP:
                lc3_eval_addr_jmp(lc3);
                break;
            case OPCODE_JSR:
                lc3_eval_addr_jsr(lc3);
                break;
            case OPCODE_BR:
                lc3_eval_addr_br(lc3);
                break;
            }
            lc3_set_state(lc3, STATE_FETCH_OP);
            break;

        /* The fourth state of the instruction cycle, the "fetch operands" state. */
        case STATE_FETCH_OP:
            switch (lc3_get_opcode(lc3)) {
            // get operands out of registers into A, B of ALU
            // or get memory for load instr.
            case OPCODE_ADD:
                lc3_fetch_op_add(lc3);

                break;
            case OPCODE_AND:
                lc3_fetch_op_and(lc3);
                // The book page 106 says current microprocessors can be done
                // simultaneously during fetch, but this simulator is old skool.
                break;
            case OPCODE_NOT:
                lc3_fetch_op_not(lc3);
                break;
            case OPCODE_TRAP:
                lc3_fetch_op_trap(lc3);
                break;
            case OPCODE_LD:
                lc3_fetch_op_ld(lc3);
                break;
            case OPCODE_LDI:
                lc3_fetch_op_ldi(lc3);
                break;
            case OPCODE_LDR:
                lc3_fetch_op_ldr(lc3);
                break;
            case OPCODE_ST:
                lc3_fetch_op_st(lc3);
                break;
            case OPCODE_STI:
                lc3_fetch_op_sti(lc3);
                break;
            case OPCODE_STR:
                lc3_fetch_op_str(lc3);
                break;
            }
            lc3_set_state(lc3, STATE_EXECUTE);
            break;

        /* The fifth state of the instruction cycle, the "execute" state. */
        case STATE_EXECUTE:
            switch (lc3_get_opcode(lc3)) {
            case OPCODE_ADD:
                lc3_execute_add(lc3);
                break;
            case OPCODE_AND:
                lc3_execute_and(lc3);
                break;
            case OPCODE_NOT:
                lc3_execute_not(lc3);
                break;
            case OPCODE_TRAP:
                trap(lc3, lc3_execute_trap(lc3));
                break;
            case OPCODE_BR:
                lc3_execute_br(lc3);
                break;
            }
            lc3_set_state(lc3, STATE_STORE);
            break;

        /* The sixth state of the instruction cycle, the "store" state. */
        case STATE_STORE:
            /* Corresponding to FSM state 16. */
            switch (lc3_get_opcode(lc3)) {
            // write back to register or store MDR into memory
            case OPCODE_ADD:
                lc3_store_add(lc3);
                break;
            case OPCODE_AND:
                lc3_store_and(lc3);
                break;
            case OPCODE_NOT:
                lc3_store_not(lc3);
                break;
            case OPCODE_LD:
                lc3_store_ld(lc3);
                break;
            case OPCODE_LDI:
                lc3_store_ldi(lc3);
                break;
            case OPCODE_LDR:
                lc3_store_ldr(lc3);
                break;
            case OPCODE_ST:
                lc3_store_st(lc3);
                break;
            case OPCODE_STI:
                lc3_store_sti(lc3);
                break;
            case OPCODE_STR:
                lc3_store_str(lc3);
                break;
            case OPCODE_LEA:
                lc3_store_lea(lc3);
                break;
            }
            is_cycle_complete = TRUE;
            lc3_set_state(lc3, STATE_FETCH);
            break;
        } // end switch (state)
    }     // end while (isCycleComplete)
} // end controller()

/*
 * This function that determines and executes the appropriate trap routine based
 * on the trap vector passed.
 */
void trap(lc3_p lc3, word_t vector) {
    char c;

    switch (vector) {
    case TRAP_VECTOR_X25:
        /** HALT */
        lc3_trap_x25(lc3);
        break;
    case TRAP_VECTOR_X20:
        /** GETC */
        c = display_monitor_get_input();
        lc3_trap_x20(lc3, c);
        break;
    case TRAP_VECTOR_X21:
        /** OUT */
        c = lc3_trap_x21(lc3);
        display_monitor_print_output(c);
        break;
    case TRAP_VECTOR_X22:
        /** PUTS */
        c = lc3_trap_x22(lc3);
        while (c != '\0') {
            display_monitor_print_output(c);
            c = lc3_trap_x22(lc3);
        }
        break;
    }
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


