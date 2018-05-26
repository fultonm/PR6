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
#include <unistd.h>

#include "display.h"
#include "lc3.h"
#include "slc3.h"

/** Allows the display to edit memory */
void prompt_edit_mem(lc3_p);

/** Returns a non-null pointer to a (hopefully) hex file */
void prompt_load_file_display(lc3_p);

/** Prompt from the terminal for a file if one wasn't specified in the arguments */
void prompt_load_file_terminal(lc3_p, int, char *[]);

/** The main instruction cycle control flow */
void controller(lc3_p, display_p);

/** Coordinates TRAP functionality between the Display and LC3 */
void trap(display_p, lc3_p, word_t);

/** Opens a file with the given file name */
FILE *open_file(char *);

/** This function allows for the loading of hex files into memory. */
void load_file_to_memory(lc3_p lc3, FILE *file);

/** Main method for the LC-3 Emulator.
 *
 * The command line argument passed in is what will be populated into
 * the LC-3's memory at runtime. For example, if the program is run with
 * input "0x1694", that hexadecimal value will be stored into the instruction
 * register (IR) and can be thought of in binary as 0001 0110 1001 0100, which
 * (based on the four highest-order bits) is an ADD instruction for the LC-3
 * (from its instruction set). */
int main(int argc, char *argv[]) {
    /** Create and initialze the LC3 object */
    lc3_p lc3 = lc3_create();

    /** Prompt from the terminal for a file if one wasn't specified in the arguments */
    prompt_load_file_terminal(lc3, argc, argv);

    /** Create and initialize the Display object */
    display_p disp = display_create();

    /** Create a snapshot of LC3's components' current parameters and pass this struct by
     * value into the Display component for displaying. This ensures the display only
     * receieves a copy of all values in the LC3 and makes it impossible for Display to
     * modify the LC3 without calling the appropriate methods. */
    const lc3_snapshot_t machine_snapshot = lc3_get_snapshot(lc3);
    display_update(disp, machine_snapshot);
    display_result_t result = display_loop(disp, machine_snapshot);

    /* If the user has selected MONITOR_STEP, then lets continue executing the
     * LC-3 */
    while (result != DISPLAY_QUIT) {
        switch (result) {
        case DISPLAY_EDIT_MEM:
            prompt_edit_mem(lc3);
            break;
        case DISPLAY_LOAD:
            prompt_load_file_display(lc3);
            break;
        case DISPLAY_STEP:
            if (lc3_is_halted(lc3) == FALSE) {
                controller(lc3, disp);
            }
            break;
        case DISPLAY_RUN:
            while (lc3_is_halted(lc3) == FALSE &&
                   display_has_breakpoint(disp, lc3_get_pc(lc3)) == FALSE) {
                /** Run through the controller for this instruction */
                controller(lc3, disp);
                /** Update the display with new information (but don't wait for a
                 * keystroke) */
                display_update(disp, lc3_get_snapshot(lc3));
                /** Sleep for 5 milliseconds */
                usleep(5000);
            }
        }
        result = display_loop(disp, lc3_get_snapshot(lc3));
    }

    /* Memory cleanup. */
    display_destroy(disp);
    lc3_destroy(lc3);

    return 0;
}

/** Prompt from and loads a hex file using the regular terminal before Display is loaded */
void prompt_load_file_terminal(lc3_p lc3, int argc, char *argv[]) {
    /** The char array used to store the hex file's name */
    char input_file_name[80];
    /** The pointer to the file on disk */
    FILE *file_ptr;
    /*
     * If there is an argument, attempt to use it first as the file name.
     * Example file name: "/hex/HW3.hex"
     */
    if (argc > 1) {
        if (argc > 2) {
            printf("Too many arguments supplied. The first argument will be treated as a file "
                   "name.\n");
        }
        file_ptr = open_file(argv[1]);
        while (file_ptr == NULL) {
            printf("File not found. Enter a file name: ");
            scanf("%s", input_file_name);
            file_ptr = open_file(input_file_name);
        }
        load_file_to_memory(lc3, file_ptr);
    }
}

/** Prompts for and loads a hex file */
void prompt_load_file_display(lc3_p lc3) {
    char user_input[64];
    display_get_file_name(user_input, sizeof(user_input)/sizeof(user_input[0]));
    FILE* file_ptr = open_file(user_input);
    while (file_ptr == NULL) {
        display_get_file_error(user_input, sizeof(user_input)/sizeof(user_input[0]));
        file_ptr = open_file(user_input);
    }
    load_file_to_memory(lc3, open_file(user_input));
}

/** Allows the display to edit memory */
void prompt_edit_mem(lc3_p lc3) {
    char address_input[6];
    display_edit_mem_get_address(address_input);
    word_t address = get_word_from_string(address_input);
    char data_input[6];
    display_edit_mem_get_data(data_input, address_input);
    word_t data = get_word_from_string(data_input);
    lc3_set_memory(lc3, address, data);
}

/*
 * The controller method of the LC-3. This contains much of the complete
 * instruction cycle of the LC-3 */
void controller(lc3_p lc3, display_p disp) {
    /** This is set to true at the end of the STORE phase and allows execution control to
     * be passed back to the main loop */
    bool_t is_cycle_complete = FALSE;

    /** Ensuring that CPU pointer being passed into the controller is valid. */
    if (!lc3) {
        exit(1);
    }

    /** Beginning instruction cycle. */
    lc3_set_state(lc3, STATE_FETCH);

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
            case OPCODE_LEA:
                lc3_eval_addr_lea(lc3);
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
            case OPCODE_ADD:
                lc3_fetch_op_add(lc3);
                break;
            case OPCODE_AND:
                lc3_fetch_op_and(lc3);
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
                trap(disp, lc3, lc3_execute_trap(lc3));
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
            case OPCODE_JMP:
                lc3_store_jmp(lc3);
                break;
            case OPCODE_JSR:
                lc3_store_jsr(lc3);
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
            case OPCODE_NOT:
                lc3_store_not(lc3);
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
void trap(display_p disp, lc3_p lc3, word_t vector) {
    char c;

    switch (vector) {
    case TRAP_VECTOR_X25:
        /** HALT */
        lc3_trap_x25(lc3);
        break;
    case TRAP_VECTOR_X20:
        /** GETC */
        c = display_get_input(disp);
        lc3_trap_x20(lc3, c);
        break;
    case TRAP_VECTOR_X21:
        /** OUT */
        c = lc3_trap_x21(lc3);
        display_print_output(disp, c);
        break;
    case TRAP_VECTOR_X22:
        /** PUTS */
        c = lc3_trap_x22(lc3);
        while (c != '\0') {
            display_print_output(disp, c);
            c = lc3_trap_x22(lc3);
        }
        break;
    }
}

/** This function will allow the opening of files */
FILE *open_file(char *file_name) {
    /* Attempt to open file. If file isn't found or otherwise null, allow user to
       press enter to return to main program of the menu. */
    FILE *input_file_pointer;
    input_file_pointer = fopen(file_name, "r");
    return input_file_pointer;
}

/** This function allows for the loading of hex files into memory. */
void load_file_to_memory(lc3_p lc3, FILE *file) {
    char line[8];
    fgets(line, sizeof(line), file);

    /** Set the starting address */
    word_t data = strtol(line, NULL, 16);
    lc3_set_starting_address(lc3, data);

    /* Read through file line by line and store to CPU memory. */
    int i = 0;
    while (fscanf(file, "%hx", &data) != EOF) {
        lc3_set_memory(lc3, lc3_get_starting_address(lc3) + i, data);
        i += 1;
    }

    if (lc3_has_file_loaded(lc3) == FALSE) {
        lc3_toggle_file_loaded(lc3);
    }
}
