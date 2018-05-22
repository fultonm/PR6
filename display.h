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
typedef struct cpu_t *cpu_p;

typedef struct display_loop_result_t {
    int user_action;
    bool breakpoint;
} display_loop_result_t;

char load_file_input[80];

display_p display_create(const lc3_snapshot_t);
void display_reset(display_p);
void display_update(display_p, lc3_snapshot_t);
int display_loop(display_p);
int display_destroy(display_p);
char display_get_input(display_p);
void display_print_output(display_p, char);

#endif
