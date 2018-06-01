/**
 *  LC-3 Simulator
 *  Final Project (Project #6)
 *  TCSS 372 - Computer Architecture
 *  Spring 2018
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

#ifndef ALU_H
#define ALU_H

#include "global.h"

typedef struct alu_t *alu_p;

/** Allocates and initializes a new ALU module */
alu_p alu_create();

/** Reinitializes the ALU without reallocation */
void alu_reset(alu_p alu);

/** Sets the ALU fields to default values */
void alu_initialize(alu_p alu);

/** Deallocates the ALU */
void alu_destroy(alu_p alu);

/** Takes a snapshot of the ALU data for debugging or display purposes */
const alu_snapshot_t alu_get_snapshot(alu_p);

/** Load ALU source register 1 */
void alu_load_sr1(alu_p, word_t data);

/** Load ALU source register 2 */
void alu_load_sr2(alu_p, word_t data);

/** Fetch the result from ALU */
word_t alu_fetch_result(alu_p);

/** Execute ADD operation on loaded SR1 and SR2 values */
void alu_add(alu_p);

/** Execute AND operation on loaded SR1 and SR2 values */
void alu_and(alu_p);

/** Execute NOT operation on loaded SR1 value */
void alu_not(alu_p);

#endif
