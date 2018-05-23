#ifndef STACK_H
#define STACK_H

#include "global.h"

typedef struct node_t *node_p;

/** Pushes SR contents onto stack */
void push();

/** Pops top element of stack into DR */
void pop();

#endif
