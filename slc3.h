/* 
 * LC-3 Simulator Simulator
 * Contributors: Mike Fulton, Sam Brendel, Logan Stafford, Enoch Chan
 * TCSS372 - Computer Architecture - Spring 2018
 *
 * LC3 Simulator Module Header
 * 
 * This a terminal-based program that emulates the the 16-bit LC-3
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

#include <stdio.h>
#include <stdbool.h>

#ifndef LC3
#define LC3

/* Register Index Addresses */
#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7

/* Machine Cycle States */
#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5
#define BREAK 6

/* Instruction States */
#define ADD 1
#define AND 5
#define NOT 9
#define BR 0
#define JMP 12
#define RET 12
#define JSR 4
#define TRAP 15
#define LD 2
#define LDR 6 							// 0110 0000 0000 0000
#define LEA 14
#define ST 3
#define STR 7 							// 0111 0000 0000 0000

/* Mask value constants. */
#define MASK_OPCODE 61440	 			// 1111 0000 0000 0000
#define MASK_DR 3584		 			// 0000 1110 0000 0000
#define MASK_SR1 448		 			// 0000 0001 1100 0000
#define MASK_SR2 7			 			// 0000 0000 0000 0111
#define MASK_PCOFFSET11 2047 			// 0000 0111 1111 1111
#define MASK_PCOFFSET9 511   			// 0000 0001 1111 1111
#define MASK_PCOFFSET6 63				// 0000 0000 0011 1111
#define MASK_TRAPVECT8 255  			// 0000 0000 1111 1111
#define MASK_BIT11 2048				 	// 0000 1000 0000 0000
#define MASK_BIT5 32		 			// 0000 0000 0010 0000
#define MASK_IMMED5 31		 			// 0000 0000 0001 1111
#define MASK_NZP 3584		 			// 0000 1110 0000 0000
#define MASK_CC_N 7
#define MASK_CC_Z 5
#define MASK_CC_P 1
#define MASK_NEGATIVE_IMMEDIATE 0xFFE0  //1111 1111 1110 0000
#define MASK_NEGATIVE_PCOFFSET11 0xF800 //1111 1000 0000 0000
#define MASK_NEGATIVE_PCOFFSET9 0xFE00  //1111 1110 0000 0000
#define MASK_NEGATIVE_PCOFFSET6 0xFFC0  //1111 1111 1100 0000

/* Condition code constants. */
#define CONDITION_N 4   				// 0000 1000 0000 0000
#define CONDITION_Z 2   				// 0000 0100 0000 0000
#define CONDITION_P 1   				// 0000 0010 0000 0000
#define CONDITION_NZ 6  				// 0000 1100 0000 0000
#define CONDITION_NP 5  				// 0000 1010 0000 0000
#define CONDITION_ZP 3  				// 0000 0110 0000 0000
#define CONDITION_NZP 7 				// 0000 1110 0000 0000

/* Bitshifting value constants */
#define BITSHIFT_OPCODE 12
#define BITSHIFT_DR 9
#define BITSHIFT_CC 9
#define BITSHIFT_SR1 6
#define BITSHIFT_BIT5 5
#define BITSHIFT_BIT11 11
#define BITSHIFT_CC_BIT3 2
#define BITSHIFT_CC_BIT2 1
#define BITSHIFT_NEGATIVE_IMMEDIATE 4
#define BITSHIFT_NEGATIVE_PCOFFSET11 10
#define BITSHIFT_NEGATIVE_PCOFFSET9 8
#define BITSHIFT_NEGATIVE_PCOFFSET6 5

/* Trap vector constants. */
#define TRAP_VECTOR_X20 0x20
#define TRAP_VECTOR_X21 0x21
#define TRAP_VECTOR_X22 0x22
#define TRAP_VECTOR_X25 0x25

/* Bit value constants. */
#define BIT_IMMED 16					// 0000 0000 0001 0000
#define BIT_PCOFFSET11 1024 			// 0000 0100 0000 0000
#define BIT_PCOFFSET9 256   			// 0000 0001 0000 0000
#define BIT_PCOFFSET6 32				// 0000 0000 0010 0000

/* Other program constants */
#define NUM_OF_BITS 16
#define NUM_OF_INSTRUCTIONS 6
#define NUM_OF_REGISTERS 8
#define NUM_OF_MEM_BANKS 512
#define FILENAME_SIZE 200
#define STRING_SIZE 200
#define OUTPUT_LINE_NUMBER 24
#define OUTPUT_COL_NUMBER 8
#define OUTPUT_AREA_DEPTH 6
#define ADDRESS_MIN 0x3000
#define MAX_HEX_BITS 4
#define MAX_BIN_BITS 16

/* LC-3 Variables */
unsigned char file_loaded;
unsigned int opcode, dr, sr1, sr2, bit5, bit11, state, nzp;
short offset, immed;
bool is_halted;
unsigned short vector;
unsigned short starting_address;
unsigned short memory[NUM_OF_MEM_BANKS];

/* ALU Struct */
typedef struct ALU
{
	/* The ALU of the LC-3 is comprised of a two input,
	   one output general purpose arithmetic/logic multiplexer. */
	unsigned short a;	   						/* First input: A */
	unsigned short b;	   						/* Second input: B */
	unsigned short result; 						/* Output/result */
} *ALU_p;

/* CPU Struct */
typedef struct CPU
{
	ALU_p alu; 									/* Contains a single ALU struct. */

	/* Various CPU registers and memory. */
	unsigned short registers[NUM_OF_REGISTERS]; /* CPU Registers */
	unsigned short state;						/* Current CPU state */
	unsigned short ir;  						/* Instruction Register */
	unsigned short pc;  						/* Program Counter */
	unsigned short ccN; 						/* Condition code "N" */
	unsigned short ccZ;							/* Condition code "Z" */
	unsigned short ccP;							/* Condition code "P" */
	unsigned short mar; 						/* Memory Address Register */
	unsigned short mdr; 						/* Memory Data Register */
	unsigned short dr;  						/* Destination Register */
	unsigned short sr1; 						/* Source 1 Register */
	unsigned short sr2; 						/* Source 2 Register */
	unsigned short base_registers;				/* The CPU base registers */
	unsigned short alu_a; 						/* Input A of the ALU */
	unsigned short alu_b; 						/* Input B of the ALU */
	unsigned short alu_result; 					/* Output result of the ALU */
} *CPU_p;

/* LC-3 Function Definitions */
void controller(CPU_p cpu);
void lc3_init(CPU_p cpu);
void int16_to_binary_IR_contents(int *binary_IR_contents, unsigned short input_number);
void trap(unsigned short, CPU_p cpu);
void set_condition_code(int input_number, CPU_p cpu);
void print_binary_form(unsigned value);
void handle_user_input(CPU_p cpu);
void display_display_monitor(CPU_p cpu);
void load_file_to_memory(CPU_p cpu, FILE *input_file_pointer);
void setCC(unsigned short, CPU_p cpu);
bool branch_enabled(unsigned short, CPU_p);
short SEXT(unsigned short, int);
unsigned int translate_memory_address(unsigned int input_address);
FILE *open_file(char *input_file_name);

#endif