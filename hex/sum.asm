; Enoch Chan
; TCSS 372
; Simple LC3 program that takes a number of arguments and prints their sum.

				.ORIG x3000

				AND R4, R4, #0				; Clear sum
				LEA R0, ARGS_PROMPT			; Load arguments prompt
				PUTS						; Print the string
				GETC						; Listen for character
				LD R1, NEG_ASCII_NUM		; Load negative ascii number (-48)
				ADD R0, R0, R1				; Subtract ascii number 0
				LEA R1, ARGS_STORE			; Load address for number of arguments
				STR R0, R1, #0				; Store number of arguments
				LD R2, ARGS_STORE			; Load R2 with number of arguments
COUNT			LEA R0, ENTER_PROMPT		; Load enter prompt
				PUTS						; Print the string
				GETC						; Listen for character
				LD R1, NEG_ASCII_NUM		; Load negative ascii number (-48)
				ADD R0, R0, R1				; Subtract ascii number 0
				ADD R0, R0, R0; PUSH R0						; Push argument onto stack
				ADD R2, R2, #-1				; Decrement argument counter
				BRp COUNT					; Push arguments until done
				LEA R2, ARGS_STORE			; Reload the number of arguments
SUM				ADD R3, R3, R3; POP R3						; Pop first argument off into R3
				ADD R4, R4, R3				; Add argument to sum
				ADD R2, R2, #-1				; Decrement argument counter
				BRp SUM						; Pop arguments until done
				LEA R0, RESULT_PROMPT		; Load result prompt
				PUTS						; Print the string
				ADD R0, R4, #0				; Load R0 with sum
				OUT							; Output the sum

NEG_ASCII_NUM   .FILL               #-48                                    ; A negative 48.
ARGS_PROMPT		.STRINGZ "How many arguments(0-10)? "
ENTER_PROMPT	.STRINGZ "\nEnter a number(0-9): "
RESULT_PROMPT	.STRINGZ "\nSum: "
ARGS_STORE		.BLKW 1

				.END
