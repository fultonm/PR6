/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
 * 
 *  Display Module Header File
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

#ifndef DISPLAY_MONITOR_H_
#define DISPLAY_MONITOR_H_

#include "slc3.h"

#define DISPLAY_QUIT 0
#define DISPLAY_LOAD 1
#define DISPLAY_SAVE 2
#define DISPLAY_STEP 3
#define DISPLAY_RUN 4
#define DISPLAY_EDIT_MEM 5
#define DISPLAY_NO_ACTION 6

typedef int display_result_t;

typedef struct display_t *display_p;
typedef struct cpu_t *cpu_p;

/** Allocates and initializes the Display */
display_p display_create();

/** Reinitializes the display by distruction and reallocation */
void display_reset(display_p);

/** Updates the display with the specified snapshot. Does not wait for the user to give a
 * command before returning control back to the LC3 simulator */
void display_update(display_p, const lc3_snapshot_t);

/** Main loop for the display. Waits for the user to select a command */
display_result_t display_loop(display_p, const lc3_snapshot_t);

/** Deallocate the Display */
int display_destroy(display_p);

/** Get input from the console */
char display_get_input(display_p);

/** Prompt for a hex file name to load */
void display_get_file_name(char *, int);

/** Reprompt for another file name since the last one was an error or didn't exist */
void display_get_file_error(char *, int);

/** Let the user know their file input was accepted */
void display_get_file_success(char *);

/** Prompt for a hex file name to load */
void display_save_file_name(char *, int);

/** Reprompt for another file name since there was an error writing to file name location */
void display_save_file_error(char *, int);

/** Let the user know their file has been successfully saved */
void display_save_file_success(char *);

/** Prompts for the address of the memory we will edit */
void display_edit_mem_get_address(char *);

/** Prompts for the data the memory location will be set to. */
void display_edit_mem_get_data(char *data_input, char *address_input);

/** Let the user know they have successfully edited the memory and show them the memory location */
void display_edit_mem_success(display_p, char *address_input, word_t address);

/** Print a general display message (not console output) */
void print_message(const char *message, char *arg);

/** Print to the output console */
void display_print_output(display_p, char);

/** Returns whether the Display has a breakpoint set at the specified address */
bool_t display_has_breakpoint(display_p, word_t);

#endif
