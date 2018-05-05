/* LC-3 Emulator
 * 
 * Date: May 2018
 *
 * This a terminal-based program that emulates the low-level functions of the 16-bit LC-3 
 * machine based a finite state machine (FSM) interpretation of its operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "slc3.h"
#include "display_monitor.h"

/* Function Definitions */
void controller(CPU_p cpu);
void cpu_init(CPU_p cpu);
void int16_to_binary_IR_contents(int *binary_IR_contents, unsigned short input_number);
void trap(CPU_p cpu);
void set_condition_code(int input_number, CPU_p cpu);
void print_binary_form(unsigned value);
void handle_user_input(CPU_p cpu);
void display_display_monitor(CPU_p cpu);
void load_file_to_memory(CPU_p cpu, FILE *input_file_pointer);
int binary_IR_contents_to_int16(int *binary_IR_helper_array, int *binary_IR_contents, int start, int length);
int get_destination_register(int *binary_IR_contents, int *binary_IR_helper_array);
int get_source1_register(int *binary_IR_contents, int *binary_IR_helper_array);
int get_source2_register(int *binary_IR_contents, int *binary_IR_helper_array);
int get_pcoffset9(int *binary_IR_contents, int *binary_IR_helper_array);
int get_opcode(int *binary_IR_contents, int *binary_IR_helper_array);
int get_immediate_mode(int *binary_IR_contents, int *binary_IR_helper_array);
int get_immediate_value(int *binary_IR_contents, int *binary_IR_helper_array);
int get_jsr_immediate_mode(int *binary_IR_contents, int *binary_IR_helper_array);
int get_jsr_immediate_value(int *binary_IR_contents, int *binary_IR_helper_array);
int get_branch_enabled(int *binary_IR_contents, int *binary_IR_helper_array, CPU_p cpu);
int get_trap_vector(int *binary_IR_contents, int *binary_IR_helper_array);

FILE *open_file();

/* 
 * Main method for the LC-3 Emulator. 
 * 
 * The command line argument passed in is what will be populated into 
 * the LC-3's memory at runtime. For example, if the program is run with
 * input "0x1694", that hexadecimal value will be stored into the instruction register (IR)
 * and can be thought of in binary as 0001 0110 1001 0100, which (based on the
 * four highest-order bits) is an ADD instruction for the LC-3 (from its instruction set).
 */
int main(int argc, char *argv[])
{
	file_loaded = 0;

	/* Creating and initializing a CPU struct. */
	CPU_p cpu = (CPU_p) calloc(1, sizeof(struct CPU));
	if (!cpu)
		return 1;
	cpu_init(cpu);

	/* Initialize and run the LC-3 from the debug monitor */
	display_monitor_init(cpu);
/*
	char input_ch = display_monitor_get_input();
	char input_ch2 = display_monitor_get_input();
	char input_str[3];
	input_str[0] = input_ch;
	input_str[1] = input_ch2;
	input_str[2] = '\0';
	display_monitor_print_output(input_str[0]);
	display_monitor_print_output(input_str[1]);*/

	int monitor_return = display_monitor_loop(cpu);
	

	/* If the user has selected MONITOR_STEP, then lets continue executing the LC-3 */
	while (monitor_return != MONITOR_QUIT)
	{
		switch (monitor_return)
		{
		case MONITOR_NO_OP:
			// printf("A no-op was passed... this shouldn't actually happen.");
			/* Fall-through to update... */
			break;

		case MONITOR_LOAD:
			load_file_to_memory(cpu, open_file());
			break;

		case MONITOR_STEP:
			if (!cpu->halted)
			{
				controller(cpu);
			}
			break;
		}
		monitor_return = display_monitor_loop(cpu);
	}

	/* Memory cleanup. */
	display_monitor_destroy();
	free(cpu);

	return 0;
}

/* 
 * This method initializes a passed in CPU object (technically a pointer to one)
 * by setting all of it's regsiter/state/memory values to zero.
 */
void cpu_init(CPU_p cpu)
{
	int i;
	for (i = 0; i < NUM_OF_REGISTERS; i++)
	{
		cpu->registers[i] = 0;
	}
	for (i = 0; i < NUM_OF_MEM_BANKS; i++)
	{
		cpu->memory[i] = 0;
	}
	cpu->instruction_register = 0;
	cpu->program_counter = 0;
	cpu->memory_data_register = 0;
	cpu->memory_address_register = 0;
	cpu->condition_code = 0;
	cpu->state = 0;
	cpu->alu_a = 0;
	cpu->alu_b = 0;
	cpu->alu_result = 0;
	cpu->trap_vector = 0;
	cpu->opcode = 0;
	cpu->destination_register = 0;
	cpu->source1_register = 0;
	cpu->source2_register = 0;
	cpu->branch_enabled = 0;
	cpu->immediate_mode = 0;
	cpu->jsr_immediate_mode = 0;
	cpu->pc_offset9 = 0;
	cpu->halted = 0;
}

/* 
 * The controller method of the LC-3. This contains much of the complete instruction 
 * cycle of the LC-3.
 */
void controller(CPU_p cpu)
{
	/* Ensuring that CPU pointer being passed into the controller is valid. */
	if (!cpu)
	{
		exit(1);
	}

	/* Creating integers array to store a binary representation of
	   the contents of the CPU's instruction register. */
	int *binary_IR_contents = (int *)malloc(sizeof(int) * NUM_OF_BITS);
	int *binary_IR_helper_array = (int *)malloc(sizeof(int) * NUM_OF_BITS);

	/* Beginning instruction cycle. */
	cpu->state = FETCH;
	int i;
	while (cpu->state != BREAK)
	{
		switch (cpu->state)
		{
		/* The first state of the instruction cycle, the "fetch" state. */
		case FETCH:

			/* Corresponding to FSM microstates 18, 33, and 35. */
			cpu->instruction_register = cpu->memory[cpu->program_counter];
			cpu->program_counter++;

			/* Moving to next state. */
			cpu->state = DECODE;
			break;

		/* The second state of the instruction cycle, the "decode" state. */
		case DECODE:
			/* Corresponding to FSM state 32. Most of these decoding functions are
				   delegated to helper functions. */
			int16_to_binary_IR_contents(binary_IR_contents, cpu->instruction_register);
			cpu->opcode = get_opcode(binary_IR_contents, binary_IR_helper_array);
			cpu->destination_register = get_destination_register(binary_IR_contents, binary_IR_helper_array);
			cpu->source1_register = get_source1_register(binary_IR_contents, binary_IR_helper_array);
			cpu->source2_register = get_source2_register(binary_IR_contents, binary_IR_helper_array);
			cpu->pc_offset9 = get_pcoffset9(binary_IR_contents, binary_IR_helper_array);
			cpu->immediate_mode = get_immediate_mode(binary_IR_contents, binary_IR_helper_array);
			cpu->immediate_value = get_immediate_value(binary_IR_contents, binary_IR_helper_array);
			cpu->branch_enabled = get_branch_enabled(binary_IR_contents, binary_IR_helper_array, cpu);
			cpu->trap_vector = get_trap_vector(binary_IR_contents, binary_IR_helper_array);

			/* Moving to next state. */
			cpu->state = EVAL_ADDR;
			break;

		/* The third state of the instruction cycle, the "evaluate address" state. */
		case EVAL_ADDR:
			switch (cpu->opcode)
			{
			/* Perform the evaluate address operations for the "Load" instruction. */
			case LD:
				cpu->memory_address_register = cpu->program_counter + cpu->pc_offset9;
				break;

			/* Perform the evaluate address operations for the "Store" instruction. */
			case ST:
				cpu->memory_address_register = cpu->program_counter + cpu->pc_offset9;
				break;

			/* Perform the evaluate address operations for the "Trap" instruction. */
			case TRAP:
				cpu->memory_address_register = cpu->trap_vector;
				break;

			/* Perform the evaluate address operations for the "Branch" instruction. */	
			case BR:
				if (cpu->branch_enabled)
				{
					cpu->program_counter = cpu->program_counter + cpu->pc_offset9;
				}
				break;

			/* Perform the evaluate address operations for the "Jump to Subroutine" instruction. */
			case JSR:
				cpu->memory_address_register = cpu->program_counter + cpu->pc_offset9;
				break;

			/* Perform the evaluate address operations for the "Load effective address" instruction. */
			case LEA:
				cpu->memory_address_register = cpu->program_counter + cpu->pc_offset9;
				cpu->state = FETCH;
				break;

			/* Perform the evaluate address operations for the "Return from interrupt" instruction. */
			case RET:
				cpu->memory_address_register = cpu->program_counter + cpu->pc_offset9;
				break;
			}

			/* Moving to next state. */
			cpu->state = FETCH_OP;
			break;

		/* The fourth state of the instruction cycle, the "fetch operands" state. */
		case FETCH_OP:
			switch (cpu->opcode)
			{
			/* Perform the fetch operands operations for the "Add" instruction. */
			case ADD:
				cpu->alu_a = cpu->registers[cpu->source1_register];
				/* If immediate mode is enabled, set B to that immediate value. */
				if (cpu->immediate_mode)
				{
					cpu->alu_b = cpu->immediate_value;
				}
				else
				{
					cpu->alu_b = cpu->registers[cpu->source2_register];
				}
				break;

			/* Perform the fetch operands operations for the "Add" instruction. */
			case AND:
				cpu->alu_a = cpu->registers[cpu->source1_register];

				/* If immediate mode is enabled, set B to that immediate value. */
				if (cpu->immediate_mode)
				{
					cpu->alu_b = cpu->immediate_value;
				}
				else
				{
					cpu->alu_a = cpu->registers[cpu->source2_register];
				}
				break;

			/* Perform the fetch operands operations for the "NOT" instruction. */
			case NOT:
				cpu->alu_a = cpu->registers[cpu->source1_register];
				break;
			
			/* Perform the fetch operands operations for the "Jump to subrountine " instruction. */
			case JSR:
				cpu->alu_a = cpu->registers[cpu->source1_register];
				/* If immediate mode is enabled, set B to that immediate value. */
				if (cpu->jsr_immediate_mode)
				{
					cpu->alu_b = cpu->jsr_immediate_value;
				}
				else
				{
					cpu->alu_b = cpu->registers[cpu->base_registers];
				}
				break;

			/* Perform the fetch operands operations for the "Trap" instruction. */
			case TRAP:
				cpu->memory_data_register = cpu->memory[cpu->memory_address_register];
				cpu->registers[7] = cpu->program_counter;
				break;

			/* Perform the fetch operands operations for the "Load" instruction. */
			case LD:
				cpu->memory_data_register = cpu->memory[cpu->memory_address_register];
				break;

			/* Perform the fetch operands operations for the "Store" instruction. */
			case ST:
				cpu->memory_data_register = cpu->registers[cpu->destination_register];
				break;

			/* Perform the fetch operands operations for the "Jump" instruction. */
			case JMP:
				cpu->base_registers = cpu->registers[cpu->source1_register];
				break;
			}

			/* Moving to next state. */
			cpu->state = EXECUTE;
			break;

		/* The fifth state of the instruction cycle, the "execute" state. */
		case EXECUTE:
			switch (cpu->opcode)
			{
			/* Perform the execute operations for the "Add" instruction. */
			case ADD:
				cpu->alu_result = cpu->alu_a + cpu->alu_b;
				break;
			
			/* Perform the execute operations for the "And" instruction. */
			case AND:
				cpu->alu_result = cpu->alu_a & cpu->alu_a;
				break;

			/* Perform the execute operations for the "Not" instruction. */			
			case NOT:
				cpu->alu_result = ~cpu->alu_a;
				break;

			/* Perform the execute operations for the "Trap" instruction. */
			case TRAP:
				cpu->program_counter = translate_memory_address(cpu->memory_data_register);
				trap(cpu);
				break;

			/* Perform the execute operations for the "Jump" instruction. */
			case JMP:
				cpu->program_counter = cpu->base_registers;
				break;

			/* Perform the execute operations for the "Jump to subroutine" instruction. */
			case JSR:
				/* If immediate mode is enabled, set B to that immediate value. */
				if (cpu->immediate_mode)
				{
					cpu->alu_b = cpu->immediate_value;
				}
				else
				{
					cpu->registers[7] = cpu->program_counter;
					cpu->program_counter = cpu->memory_address_register;
				}
				
				break;
			}

			/* Moving to next state. */
			cpu->state = STORE;
			break;

		/* The sixth state of the instruction cycle, the "store" state. */
		case STORE:
			/* Corresponding to FSM state 16. */
			switch (cpu->opcode)
			{
			/* Perform the store operations for the "Add" instruction. */
			case ADD:
				cpu->registers[cpu->destination_register] = cpu->alu_result;
				set_condition_code(cpu->alu_result, cpu);
				break;

			/* Perform the store operations for the "And" instruction. */
			case AND:
				cpu->registers[cpu->destination_register] = cpu->alu_result;
				set_condition_code(cpu->alu_result, cpu);
				break;

			/* Perform the store operations for the "Not" instruction. */
			case NOT:
				cpu->registers[cpu->destination_register] = cpu->alu_result;
				set_condition_code(cpu->alu_result, cpu);
				break;

			/* Perform the store operations for the "Load" instruction. */
			case LD:
				cpu->registers[cpu->destination_register] = cpu->memory_data_register;
				set_condition_code(cpu->memory_data_register, cpu);
				break;

			/* Perform the store operations for the "Store" instruction. */
			case ST:
				cpu->memory[cpu->memory_address_register] = cpu->memory_data_register;
				break;

			/* Perform the store operations for the "Jump to subroutine" instruction. */
			case JSRR:
				break;

			/* Perform the store operations for the "Load effective address instruction. */
			case LEA:
				cpu->alu_a = cpu->registers[cpu->source1_register];
				break;
			}

			/* Moving to next state. */
			cpu->state = BREAK;
			break;
		}
	}

	/* Memory cleanup. */
	free(binary_IR_contents);
	free(binary_IR_helper_array);
}

/*
 * This function assignes a binary representation of a given 16-bit integer
 * in the form of a one-dimensional array.
 */
void int16_to_binary_IR_contents(int *binary_IR_contents, unsigned short input_number)
{
	/* Ensure space in memory is clear of contents/zeroed. */
	int i;
	for (i = 0; i < NUM_OF_BITS; i++)
	{
		binary_IR_contents[i] = 0;
	}

	/* Convert the integer bit-by-bit into binary (in the array). */
	int temp_array[NUM_OF_BITS];
	for (i = 0; i < NUM_OF_BITS; i++)
	{
		temp_array[i] = input_number % 2;
		input_number = input_number / 2;
	}

	/* Reverse the order the bits and store in binary_IR_contents. */
	for (i = 0; i < NUM_OF_BITS; i++)
	{
		binary_IR_contents[15 - i] = temp_array[i];
	}
}

/*
 * This function receives a binary integer array, a target binary array, 
 * a start bit, and a length bit. With this information, the function
 * finds and returns an integer representation of a 16-bit binary subarray of
 * a given binary array based on a given start bit and bit length.
 */
int binary_IR_contents_to_int16(int *binary_IR_helper_array, int *binary_IR_contents, int start, int length)
{
	/* Ensure space in memory is clear of contents/zeroed. */
	int i;
	for (i = 0; i < NUM_OF_BITS; i++)
	{
		binary_IR_helper_array[i] = 0;
	}

	for (i = 0; i < length; i++)
	{
		binary_IR_helper_array[i] = binary_IR_contents[start + i];
	}

	/* Finally, calculate and return the integer value. */
	int final_value = 0;
	for (i = 0; i < length; i++)
	{
		final_value = final_value * 2;
		final_value += binary_IR_helper_array[i];
	}

	return final_value;
}

/*
 * This function that determines and executes the appropriate trap routine based on the
 * trap vector passed.
 */
void trap(CPU_p cpu)
{
	char c;

	switch (cpu->trap_vector)
	{
		case  0X25:
			/* Exit program state. */
			cpu->state = FETCH;
			cpu->halted = 1;
			break;

		case  0X20:
			/* getch */

			c = getc(stdin);
			break;

		case  0X21:
			/* out (same as putc or simple prinf("%c");) */
			putc(c, stdout);			
			break;

		case  0X22:
			/* puts (simple printf without /n) */
			for (int i = 0; i < cpu->registers[0]; i++) {
				if (i == '/0') {
					break;
				} else { 
					display_monitor_print_output(i);
				}
			}

			break;

	}
}

/*
 * This function will set the condition code based on the input number passed.
 */
void set_condition_code(int input_number, CPU_p cpu)
{
	if (input_number > 0)
	{
		cpu->condition_code = 1; /* P */
	}
	else if (input_number < 0)
	{
		cpu->condition_code = 4; /* N */
	}
	else
	{
		cpu->condition_code = 2; /* Z */
	}
}

/*
 * This function finds and returns the integer value of the opcode 
 * based on input memory.
 */
int get_opcode(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to the first 4 bits of memory. */
	int opcode = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 0, 4);

	return opcode;
}

/*
 * This function finds and returns the integer value of the destination register
 * based on input memory.
 */
int get_destination_register(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [4:6] in memory. */
	int destination_register = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 4, 3);

	return destination_register;
}

/*
 * This function finds and returns the integer value of the SR1 register
 * based on input memory.
 */
int get_source1_register(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [7:9] in memory. */
	int source1_register = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 7, 3);

	return source1_register;
}

/*
 * This function finds and returns the integer value of the SR2 register
 * based on input memory.
 */
int get_source2_register(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [13:15] in memory. */
	int source2_register = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 13, 3);

	return source2_register;
}

/*
 * This function finds and returns the integer value of the PC offset
 *  based on input memory.
 */
int get_pcoffset9(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [7:15] in memory. */
	int pc_offset9 = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 7, 9);

	return pc_offset9;
}

/*
 * This function finds and returns the immediate mode for 
 * the AND and ADD instructions based on the contents of the instruction register (IR).
 */
int get_immediate_mode(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bit [10] in memory. */
	int immedate_mode = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 10, 1);

	return immedate_mode;
}

/*
 * This function finds and returns the immediate mode for 
 * the JSR and the JSRR instructions based on the contents of the instruction register (IR).
 */
int get_jsr_immediate_mode(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bit [11] in memory. */
	int jsr_immedate_mode = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 11, 1);

	return jsr_immedate_mode;
}

/*
 * This function finds and returns the immediate value for
 * the AND and ADD instructions based on the contents of the instruction register (IR).
 */
int get_immediate_value(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [11:15] in memory. */
	int immediate_value = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 11, 5);

	return immediate_value;
}

/* 
 * This function finds and returns the immediate value for
 * the JSR and JSRR instructions based on the contents of the instruction register (IR).
 */
int get_jsr_immediate_value(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [10:0] in memory. */
	int jsr_immediate_value = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 10, 11);

	return jsr_immediate_value;
}

/*
 * This function determines and returns a 1 or 0 to represent if a branch 
 * should be taken or not.
 */
int get_branch_enabled(int *binary_IR_contents, int *binary_IR_helper_array, CPU_p cpu)
{
	/* Corresponds to bits [4:6] in memory. */
	binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 4, 3);

	/* Convert the condition code to an integer array for bit comparison. */
	int *condition_code = (int *)malloc(sizeof(int) * 3);
	int input_number = cpu->condition_code, i;
	int temp_array[3];

	for (i = 0; i < 3; i++)
	{
		temp_array[i] = input_number % 2;
		input_number = input_number / 2;
	}
	for (i = 0; i < 3; i++)
	{
		condition_code[2 - i] = temp_array[i];
	}

	/* Finally, determine if branch should be enabled. */
	int result = 0;
	for (i = 0; i < 3; i++)
	{
		if (binary_IR_helper_array[i] == condition_code[i])
		{
			result = 1; /* If any bit is shared between the two, set branch enabled to 1. */
		}
	}

	/* Memory cleanup. */
	free(condition_code);
	return result;
}

/*
 * This function finds and returns the trap vector value.
 */
int get_trap_vector(int *binary_IR_contents, int *binary_IR_helper_array)
{
	/* Corresponds to bits [8:15] in memory. */
	int trap_vector = binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 8, 8);

	return binary_IR_contents_to_int16(binary_IR_helper_array, binary_IR_contents, 8, 8);
}

/* 
 * This function prints a binary representation of a given unsigned integer value to the screen.
 */
void print_binary_form(unsigned value)
{
	if (value > 1)
		print_binary_form(value / 2);

	printf("%d", value % 2);
}

/*
 * This function will translate a given LC-3 user space memory address (which begins at 0x3000) into 
 * a regular memory address (which begins at 0x0).
 */
unsigned int translate_memory_address(unsigned int input_address)
{
	return input_address - 0x3000;
}

/* 
 * This function will allow the opening of files
 */
FILE *open_file()
{
	/* Attempt to open file. If file isn't found or otherwise null, allow user to press enter to 
	return to main program of the menu. */
	FILE *input_file_pointer;
	input_file_pointer = fopen(load_file_input, "r");
	if (input_file_pointer == NULL)
	{
		/* 
		 * Error checking is hard to do with this file structure... How can we get an
		 * error message back to the display_monitor? We would want an error message to
		 * appear right under the user selection options.
		 */
	}
	return input_file_pointer;
}

/*
 * This function allows for the loading of hex files into memory.
 */
void load_file_to_memory(CPU_p cpu, FILE *input_file_pointer)
{
	char line[64];
	fgets(line, sizeof(line), input_file_pointer);

	/* 
	 * Subtract 0x3000 from first hex value in file to be starting memory location
	 * Note: This requires the first line of the hex file to not be less than x3000 since
	 * this is an unsigned short. 
	 * 
	 * IDEA: Create offset variable to hold difference between given start location and x3000?
	 * This could be positive or negative value? Then compute from this value.
	 */
	unsigned short starting_address = strtol(line, NULL, 16);
	starting_address = translate_memory_address(starting_address);

	/* Read through file line by line and store to CPU memory. */
	unsigned short address;
	int result = 1;
	int i = 0;
	while (fscanf(input_file_pointer, "%hx", &address) != EOF)
	{
		cpu->memory[starting_address + i] = address;
		i += 1;
	}
	file_loaded = 1;
}