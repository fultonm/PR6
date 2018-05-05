/* LC-3 Emulator
 * 
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the 16-bit LC-3 
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

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
#define JSRR 4444
#define TRAP 15
#define LD 2
#define LEA 14
#define ST 3

/* Other Program Constants */
#define NUM_OF_BITS 16
#define NUM_OF_INSTRUCTIONS 6
#define NUM_OF_REGISTERS 8
#define NUM_OF_MEM_BANKS 512

/* CPU Struct */
#ifndef LC3
#define LC3

unsigned int translate_memory_address(unsigned int input_address);

unsigned char file_loaded;

typedef struct CPU 
{
	/* Various CPU registers and memory. */
	unsigned short registers[NUM_OF_REGISTERS];
	unsigned short memory[NUM_OF_MEM_BANKS];
	unsigned short state;
	unsigned short instruction_register;
	unsigned short program_counter;
	unsigned short condition_code;
	unsigned short alu_a;
	unsigned short alu_b;
	unsigned short alu_result;
	unsigned short memory_address_register;
	unsigned short memory_data_register;
	unsigned short destination_register;
	unsigned short source1_register;
	unsigned short source2_register;
	unsigned short base_registers;

	/** Convience methods */
	unsigned short opcode;
	unsigned short pc_offset9;
	unsigned char immediate_mode;
	unsigned short immediate_value;
	unsigned char jsr_immediate_mode;
	unsigned short jsr_immediate_value;
	unsigned short trap_vector;
	unsigned char branch_enabled;
	unsigned char halted;
} *CPU_p;

#endif