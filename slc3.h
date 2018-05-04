/*
 *  slc3.h
 *
 *  Date Due: May 2, 2018
 *  Authors:  Sam Brendel, Mike Josten
 *  Problem 5
 *  version: 4.30d
 */
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>

#ifndef SLC3_H_
#define SLC3_H_

#define MEMORY_SIZE       10000
#define FILENAME_SIZE       200
#define STRING_SIZE         200
#define OUTPUT_LINE_NUMBER   24
#define OUTPUT_COL_NUMBER     8
#define OUTPUT_AREA_DEPTH     6
#define ADDRESS_MIN      0x3000
#define MAX_HEX_BITS          4
#define MAX_BIN_BITS         16

#define FETCH     0
#define DECODE    1
#define EVAL_ADDR 2
#define FETCH_OP  3
#define EXECUTE   4
#define STORE     5

#define OP_ADD   1 // 0001 0000 0000 0000
#define OP_AND   5 // 0101 0000 0000 0000
#define OP_NOT   9 // 1001 0000 0000 0000
#define OP_TRAP 15 // 1111 0000 0000 0000
#define OP_LD    2 // 0010 0000 0000 0000
#define OP_LDR   6 // 0110 0000 0000 0000
#define OP_ST    3 // 0011 0000 0000 0000
#define OP_STR   7 // 0111 0000 0000 0000
#define OP_JMP  12 // 1100 0000 0000 0000
#define OP_BR    0 // 0000 0000 0000 0000
#define OP_JSR   4 // 0100 0000 0000 0000
#define OP_LEA  14 // 1110 0000 0000 0000

#define MASK_OPCODE  61440 // 1111 0000 0000 0000
#define MASK_DR       3584 // 0000 1110 0000 0000
#define MASK_SR1       448 // 0000 0001 1100 0000
#define MASK_SR2         7 // 0000 0000 0000 0111
#define MASK_PCOFFSET11 2047 // 0000 0111 1111 1111
#define MASK_PCOFFSET9 511 // 0000 0001 1111 1111
#define MASK_PCOFFSET6  63 // 0000 0000 0011 1111
#define MASK_TRAPVECT8 255 // 0000 0000 1111 1111
#define MASK_BIT11    2048 // 0000 1000 0000 0000
#define MASK_BIT5       32 // 0000 0000 0010 0000
#define MASK_IMMED5     31 // 0000 0000 0001 1111
#define MASK_NZP      3584 // 0000 1110 0000 0000
#define MASK_CC_N        7
#define MASK_CC_Z        5
#define MASK_CC_P        1
#define MASK_NEGATIVE_IMMEDIATE 0xFFE0 //1111 1111 1110 0000
#define MASK_NEGATIVE_PCOFFSET11 0xF800 //1111 1000 0000 0000
#define MASK_NEGATIVE_PCOFFSET9 0xFE00	//1111 1110 0000 0000
#define MASK_NEGATIVE_PCOFFSET6 0xFFC0  //1111 1111 1100 0000

#define CONDITION_N   4 // 0000 1000 0000 0000
#define CONDITION_Z   2 // 0000 0100 0000 0000
#define CONDITION_P   1 // 0000 0010 0000 0000
#define CONDITION_NZ  6 // 0000 1100 0000 0000
#define CONDITION_NP  5 // 0000 1010 0000 0000
#define CONDITION_ZP  3 // 0000 0110 0000 0000
#define CONDITION_NZP 7 // 0000 1110 0000 0000

// How many times to shift the bits.
#define BITSHIFT_OPCODE            12
#define BITSHIFT_DR                 9
#define BITSHIFT_CC                 9
#define BITSHIFT_SR1                6
#define BITSHIFT_BIT5               5
#define BITSHIFT_BIT11		   11
#define BITSHIFT_CC_BIT3            2
#define BITSHIFT_CC_BIT2            1
#define BITSHIFT_NEGATIVE_IMMEDIATE 4
#define BITSHIFT_NEGATIVE_PCOFFSET11 10
#define BITSHIFT_NEGATIVE_PCOFFSET9 8
#define BITSHIFT_NEGATIVE_PCOFFSET6 5	  

#define TRAP_VECTOR_X20  0x20
#define TRAP_VECTOR_X21  0x21
#define TRAP_VECTOR_X22  0x22
#define TRAP_VECTOR_X25  0x25

#define BIT_IMMED        16 //0000 0000 0001 0000
#define BIT_PCOFFSET11 1024// 0000 0100 0000 0000
#define BIT_PCOFFSET9   256// 0000 0001 0000 0000
#define BIT_PCOFFSET6   32 // 0000 0000 0010 0000

struct CPUType {
	unsigned short int pc;     // program counter.
	unsigned short cc;         // condition code for BR instruction.
	unsigned short int reg[8]; // registers.
	unsigned short int ir;     // instruction register.
	unsigned short mar;        // memory address register.
	unsigned short mdr;        // memory data register.
	unsigned short microState;
	unsigned short A;
	unsigned short B;
};

typedef struct CPUType CPU_p;

int controller (CPU_p *, WINDOW *);
void displayCPU(CPU_p *, int);
void zeroOut(unsigned short *array, int);
CPU_p initialize();
unsigned short ZEXT(unsigned short);
short toSign(unsigned short);
short SEXT(unsigned short, int);
void TRAP(unsigned short, CPU_p *, WINDOW *);
unsigned short getCC(unsigned short);
bool branchEnabled(unsigned short, CPU_p *);
void displayHeader();
FILE* openFileText(char *, WINDOW *);
void loadProgramInstructions(FILE *, WINDOW *);
int hexCheck(char num[]);
void cursorAtPrompt(WINDOW *, char *);
void cursorAtInput(WINDOW *, char *);
void cursorAtOutput(WINDOW *, char *);
void cursorAtCustom(WINDOW *, int, int, char *);
void clearOutput(WINDOW *);

#endif /* SLC3_H_ */
