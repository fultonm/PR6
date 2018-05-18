#include "global.h"

typedef struct cpu_t *cpu_p;

/** Allocates and initializes a new cpu module */
cpu_p cpu_create();

/** Reinitializes the cpu module without reallocation */
void cpu_reset(cpu_p);

/** Deallocates the cpu module */
void cpu_destroy(cpu_p);

/** Fetches the data at the specified regiser */
word_t cpu_get_register(cpu_p, reg_addr_t reg);

/** Sets the specified register to the given data and sets the CC */
void cpu_set_register(cpu_p, reg_addr_t reg, word_t data);

/** Fetches the high order bit of the CC representing negative */
bool_t cpu_fetch_cc_n(cpu_p);

/** Fetches the second bit of the CC representing zero */
bool_t cpu_fetch_cc_z(cpu_p);

/** Fetches the low order bit of the CC representing positive */
bool_t cpu_fetch_cc_p(cpu_p);

/** Sets the instruction register */
void cpu_set_ir(cpu_p, word_t data);

/** Gets the instruction register */
word_t cpu_get_ir(cpu_p);

/** Increments the PC by 1 and returns the new PC */
void cpu_increment_pc(cpu_p);

/** Increments/decrements the PC by the given value and returns the new PC */
void cpu_increment_pc_by_value(cpu_p, word_t value);

/** Sets the PC to a new value */
void cpu_set_pc(cpu_p, word_t data);

/** Returns the current PC */
word_t cpu_get_pc(cpu_p);

/** Returns the current MAR */
word_t cpu_get_mar(cpu_p);

/** Sets the MAR */
void cpu_set_mar(cpu_p, word_t data);

/** Returns the current MDR */
word_t cpu_get_mdr(cpu_p);

/** Sets the MDR */
void cpu_set_mdr(cpu_p, word_t data);