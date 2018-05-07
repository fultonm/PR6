/* LC-3 Emulator
 * 
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the 16-bit LC-3 
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

#include "slc3.h"

#ifndef DISPLAY_MONITOR_H_
#define DISPLAY_MONITOR_H_

#define MONITOR_QUIT 0
#define MONITOR_STEP 1
#define MONITOR_LOAD 2
#define MONITOR_NO_OP 3

char load_file_input[80];

int display_monitor_init(CPU_p);
void display_monitor_update(CPU_p);
int display_monitor_loop(CPU_p);
int display_monitor_destroy();
char display_monitor_get_input();
void display_monitor_print_output(char);

#endif