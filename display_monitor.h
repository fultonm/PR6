/* 
 * LC-3 Simulator Simulator
 * Contributors: Mike Fulton, Sam Brendel, Logan Stafford, Enoch Chan
 * TCSS372 - Computer Architecture - Spring 2018
 *
 * Display Monitor Module Header
 * 
 * This a terminal-based program that emulates the the 16-bit LC-3
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

/* Display Monitor Header Dependencies */
#include "slc3.h"

#ifndef DISPLAY_MONITOR_H_
#define DISPLAY_MONITOR_H_

/* Display Monitor Constants */
#define MONITOR_QUIT        0
#define MONITOR_LOAD        1
#define MONITOR_STEP        2
#define MONITOR_RUN         3
#define MONITOR_UPDATE      4
#define MONITOR_NO_RETURN   5

bool has_breakpoint[NUM_OF_MEM_BANKS];

/* Display Monitor Function Declarations */
int display_monitor_init(CPU_p);
int display_monitor_loop(CPU_p);
int display_monitor_destroy();
char display_monitor_get_input();
void display_monitor_update(CPU_p);
void display_monitor_get_file_name(char *input_file_name);
void display_monitor_print_output(char);
void print_message(const char *message, char *arg);

#endif