/* 
 * LC-3 Emulator
 * 
 * Author: Logan Stafford 
 * Author: Michael Fulton
 * 
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the 16-bit LC-3 
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

#include "slc3.h"

#ifndef DEBUG_MONITOR_H_
#define DEBUG_MONITOR_H_

#define MONITOR_QUIT 0
#define MONITOR_STEP 1
#define MONITOR_LOAD 2
#define MONITOR_NO_OP 3

char load_file_input[80];

int debug_monitor_init(CPU_p);
void debug_monitor_update(CPU_p);
int debug_monitor_loop(CPU_p);
int debug_monitor_destroy();
char debug_monitor_get_input();
void debug_monitor_print_output(char);

#endif