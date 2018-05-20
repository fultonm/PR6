#include "slc3.h"

typedef struct alu_t {
    word_t sr1;
    word_t sr2;
    word_t result;
} alu_t, *alu_p;

/** Allocates and initializes a new ALU module */
alu_p alu_create() {
    alu_p alu = calloc(1, sizeof(alu_t));
    initialize(alu);
    return alu;
}

/** Reinitializes the ALU without reallocation */
void alu_reset(alu_p alu) { initialize(alu); }

/** Sets the ALU fields to default values */
void initialize(alu_p alu) {
    alu->sr1 = 0;
    alu->sr2 = 0;
    alu->result = 0;
}

/** Deallocates the ALU */
void alu_destroy(alu_p alu) { free(alu); }

/** Load ALU source register 1 */
void alu_load_sr1(alu_p alu, word_t data) { alu->sr1 = data; }

/** Load ALU source register 2 */
void alu_load_sr2(alu_p alu, word_t data) { alu->sr2 = data; }

/** Fetch the result from ALU */
word_t alu_fetch_result(alu_p alu) { return alu->result; }

/** Execute ADD operation on loaded SR1 and SR2 values */
void alu_add(alu_p alu) { alu->result = alu->sr1 + alu->sr2; }

/** Execute AND operation on loaded SR1 and SR2 values. Note: "and" is a protected keyword in C
 * so we'll use "and_op" here */
void alu_and(alu_p alu) { alu->result = alu->sr1 & alu->sr2; }

/** Execute NOT operation on loaded SR1 value */
void alu_not(alu_p alu) { alu->result = ~alu->sr1; }