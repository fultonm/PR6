; Enoch Chan
; TCSS 372
; Simple LC3 program that takes a number of arguments and prints their sum.
; Note: if the resulting sum is greater than 9, the output will not display properly.

				.ORIG x3000

				LD R6, STACK_BASE			; Initialize stack pointer
				AND R4, R4, #0				; Clear sum
				LEA R0, ARGS_PROMPT			; Load arguments prompt
				PUTS						; Print the string
				JSR GET_ARGS				; Jump to subroutine
				JSR COUNT					; Jump to subroutine
				JSR SUM						; Jump to subroutine
				LEA R1, RESULT				; Result address loaded
				STR R0, R1, #0				; Store sum into result address
				LEA R0, RESULT_PROMPT		; Load result prompt
				PUTS						; Print the string
				LDR R0, R1, #0				; Move result into R0 to be printed
				ADD R0, R0, #12
				ADD R0, R0, #12
				ADD R0, R0, #12
				ADD R0, R0, #12
				OUT							; Output the sum
				HALT						;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GET_ARGS		ADD R0, R0, R0; PUSH R7
				GETC						; Listen for character
				ADD R0, R0, R0; POP R7

				ADD R0, R0, R0; PUSH R7						; Push return address for GET_ARGS subroutine
				ADD R0, R0, R0; PUSH R0						; Push argument
				JSR CONVERT					; Jump to subroutine, R7 <- return address for CONVERT
				ADD R0, R0, R0; POP R7						; Get back return address for GET_ARGS subroutine

				LEA R1, ARGS_STORE			; (13)Load address for number of arguments
				STR R0, R1, #0				; Store number of arguments
				LD R2, ARGS_STORE			; Load R2 with number of arguments
				RET
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
COUNT			LEA R0, ENTER_PROMPT		; Load enter prompt
				ADD R0, R0, R0; PUSH R7
				PUTS						; Print the string
				ADD R0, R0, R0; POP R7
				ADD R0, R0, R0; PUSH R7
				GETC						; Listen for character
				ADD R0, R0, R0; POP R7

				ADD R0, R0, R0; PUSH R7						; Push return address for COUNT subroutine
				ADD R0, R0, R0; PUSH R0						; (1F) Push argument
				JSR CONVERT					; Jump to subroutine, R7 <- return address for CONVERT
				ADD R0, R0, R0; POP R7						; Get back return address for COUNT subroutine

				ADD R0, R0, R0; PUSH R0						; Push argument onto stack
				ADD R2, R2, #-1				; Decrement argument counter
				BRp COUNT					; (24) Push arguments until done
				LD R2, ARGS_STORE			; Reload the number of arguments
				RET							;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SUM				ADD R0, R0, R0; POP R3						; Pop first argument off into R3
				ADD R4, R4, R3				; Add argument to sum
				ADD R2, R2, #-1				; Decrement argument counter
				BRp SUM						; Pop arguments until done
				ADD R0, R4, #0				; Load R0 with sum
				RET							;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CONVERT			ADD R0, R0, R0; POP R0						; Pop off argument
				LD R3, NEG_ASCII_NUM		; Load negative ascii number (-48)
				ADD R0, R0, R3				; Subtract ascii number 0
				RET							;

NEG_ASCII_NUM   .FILL               #-48				; A negative 48.
ARGS_PROMPT		.STRINGZ "How many arguments(0-5)? "
ENTER_PROMPT	.STRINGZ "\nEnter a number(0-9): "
RESULT_PROMPT	.STRINGZ "\nSum: "
ARGS_STORE		.BLKW 1									; Store number of arguments
RESULT			.BLKW 1									; The sum result
STACK_BASE		.FILL				x31FF

				.END
