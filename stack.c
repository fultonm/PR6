#include <stdio.h>
#include "stack.h"
#include "slc3.h"
#include "global.h"

// basic element in stack
typedef struct node_t
{
    int data;
    struct node_t *next;
    struct node_t *prev;
} node_t, *node_p;

node_p base = NULL;                         // base of stack
node_p top = NULL;                          // top of stack

void push()
{
    node_p cur;
    cur = base;

    if (cur == NULL)                        // stack is empty
    {                                       // add node to empty stack
        cur = malloc(sizeof(struct node_t));
        cur->data = SR;                     // assign SR value to new node
        cur->next = NULL;
        cur->prev = NULL;
        base = top = cur;                   // set stack base and top
    }
    else                                    // stack is not empty
    {
        int stackIndex = 0;                 // used to ensure stack is within limits
        while (cur->next != NULL) {
            cur = cur->next;                // iteratively move cur to top of stack
            stackIndex++;
            if (stackIndex >= STACKMAX - 1) // check for stack overflow
            {                               // subtract 1 to make room for new node
                                            // TODO add STACKMAX to global.h
                R5 = 0;                     // push failed, stack overflow
                return;
            }
        }
        // create new node and add to top of stack
        (cur->next) = malloc(sizeof(struct node_t));
        (cur->next)->data = SR;             // set data to global SR contents
        (cur->next)->next = NULL;
        (cur->next)->prev = cur;
        top = (cur->next);                  // set new top
    }
    R5 = 1;                                 // successful push
}

void pop()
{
    node_p cur;
    cur = top;

    if (cur == NULL)                            // stack is empty
    {
        R5 = 0;                                 // pop failed, stack underflow
        return;
    }
    else if (cur->prev == cur->next == NULL)    // 1 node on stack
    {
        DR = top->data;                         // store popped node data into DR
        base = top = NULL;                      // clear stack
    }
    else                                        // 2+ nodes on stack
    {
        DR = top->data;
        top = top->prev;                        // remove top node
        top->next = NULL;
    }
    R5 = 1;                                     // successful pop
}
