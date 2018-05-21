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
#define MONITOR_LOAD 1
#define MONITOR_STEP 2
#define MONITOR_RUN 3
#define MONITOR_UPDATE 4
#define MONITOR_NO_ACTION 5

typedef struct display_t *display_p;

typedef struct MonitorReturn {
    int user_action;
    bool breakpoint;
} MonitorReturn;

char load_file_input[80];

int display_init(display_p);
void display_update(display_p);
int display_loop(display_p);
int display_destroy(display_p);
char display_get_input(display_p);
void display_print_output(display_p, char);

#endif
