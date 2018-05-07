/* LC-3 Emulator
 * 
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the 16-bit LC-3 
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

#include <stdio.h>
#include <stdbool.h>

#ifndef LC3
#define LC3

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
#define LDR 6 // 0110 0000 0000 0000
#define LEA 14
#define ST 3
#define STR 7 // 0111 0000 0000 0000

/* Other Program Constants */
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

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5

#define MASK_OPCODE 61440	// 1111 0000 0000 0000
#define MASK_DR 3584		 // 0000 1110 0000 0000
#define MASK_SR1 448		 // 0000 0001 1100 0000
#define MASK_SR2 7			 // 0000 0000 0000 0111
#define MASK_PCOFFSET11 2047 // 0000 0111 1111 1111
#define MASK_PCOFFSET9 511   // 0000 0001 1111 1111
#define MASK_PCOFFSET6 63	// 0000 0000 0011 1111
#define MASK_TRAPVECT8 255   // 0000 0000 1111 1111
#define MASK_BIT11 2048		 // 0000 1000 0000 0000
#define MASK_BIT5 32		 // 0000 0000 0010 0000
#define MASK_IMMED5 31		 // 0000 0000 0001 1111
#define MASK_NZP 3584		 // 0000 1110 0000 0000
#define MASK_CC_N 7
#define MASK_CC_Z 5
#define MASK_CC_P 1
#define MASK_NEGATIVE_IMMEDIATE 0xFFE0  //1111 1111 1110 0000
#define MASK_NEGATIVE_PCOFFSET11 0xF800 //1111 1000 0000 0000
#define MASK_NEGATIVE_PCOFFSET9 0xFE00  //1111 1110 0000 0000
#define MASK_NEGATIVE_PCOFFSET6 0xFFC0  //1111 1111 1100 0000

#define CONDITION_N 4   // 0000 1000 0000 0000
#define CONDITION_Z 2   // 0000 0100 0000 0000
#define CONDITION_P 1   // 0000 0010 0000 0000
#define CONDITION_NZ 6  // 0000 1100 0000 0000
#define CONDITION_NP 5  // 0000 1010 0000 0000
#define CONDITION_ZP 3  // 0000 0110 0000 0000
#define CONDITION_NZP 7 // 0000 1110 0000 0000

// How many times to shift the bits.
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

#define TRAP_VECTOR_X20 0x20
#define TRAP_VECTOR_X21 0x21
#define TRAP_VECTOR_X22 0x22
#define TRAP_VECTOR_X25 0x25

#define BIT_IMMED 16		// 0000 0000 0001 0000
#define BIT_PCOFFSET11 1024 // 0000 0100 0000 0000
#define BIT_PCOFFSET9 256   // 0000 0001 0000 0000
#define BIT_PCOFFSET6 32	// 0000 0000 0010 0000

unsigned int translate_memory_address(unsigned int input_address);
unsigned char file_loaded;

unsigned int opcode, dr, sr1, sr2, bit5, bit11, state, nzp; // fields for the IR
short offset, immed;
bool isHalted;
unsigned short vector;

typedef struct ALU
{
	unsigned short a;	  // input.
	unsigned short b;	  // input.
	unsigned short result; // result.
} * ALU_p;

/* CPU Struct */
typedef struct CPU
{
	ALU_p alu;
	/* Various CPU registers and memory. */
	unsigned short registers[NUM_OF_REGISTERS];
	unsigned short state;
	unsigned short ir;  // instruction_register
	unsigned short pc;  // program_counter
	unsigned short cc;  // condition_code
	unsigned short mar; // memory_address_register
	unsigned short mdr; // memory_data_register
	unsigned short dr;  // destination_register
	unsigned short sr1; // source1_register
	unsigned short sr2; // source2_register
	unsigned short base_registers;
	unsigned short alu_a;
	unsigned short alu_b;
	unsigned short alu_result;

	/** Convience methods */ /*
	unsigned short opcode;
	unsigned short pc_offset9;
	unsigned char immediate_mode;
	unsigned short immediate_value;
	unsigned char jsr_immediate_mode;
	unsigned short jsr_immediate_value;
	unsigned short trap_vector;
	unsigned char branch_enabled;
	unsigned char halted;*/
} * CPU_p;

/* Memory modules */
unsigned short memory[NUM_OF_MEM_BANKS];

/* Function Definitions */
void controller(CPU_p cpu);
void lc3_init(CPU_p cpu);
void int16_to_binary_IR_contents(int *binary_IR_contents, unsigned short input_number);
void trap(unsigned short, CPU_p cpu);
void set_condition_code(int input_number, CPU_p cpu);
void print_binary_form(unsigned value);
void handle_user_input(CPU_p cpu);
void display_display_monitor(CPU_p cpu);
void load_file_to_memory(CPU_p cpu, FILE *input_file_pointer);
short SEXT(unsigned short, int);
unsigned short getCC(unsigned short);
bool branchEnabled(unsigned short, CPU_p);

// int binary_IR_contents_to_int16(int *binary_IR_helper_array, int *binary_IR_contents, int start, int length);
// int get_destination_register(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_source1_register(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_source2_register(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_pcoffset9(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_opcode(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_immediate_mode(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_immediate_value(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_jsr_immediate_mode(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_jsr_immediate_value(int *binary_IR_contents, int *binary_IR_helper_array);
// int get_branch_enabled(int *binary_IR_contents, int *binary_IR_helper_array, CPU_p cpu);
// int get_trap_vector(int *binary_IR_contents, int *binary_IR_helper_array);

#endif