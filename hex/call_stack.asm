; A program for testing the push/pop unused opcode
; Michael Fulton
; TCSS 372


; Program
                .ORIG x3000             ; Start at x3000
                LEA R0, RECUR_CT_PROMPT ; Load Prompt for recurse count
                PUTS                    ; Print the string
                JSR RECUR_CT            ; Jump to get recurse count routine.

                AND R1, R1, #0          ; R1 is now the number of times recursed.
                LD R2, RECUR_CT_INT     ; R2 is now the number of times to recurse.
                NOT R2, R2              ; NOT R2 and add 1 to make it negative. (two's complement)
                ADD R2, R2, #1          ; R2 is now the negative of the number of times to recurse
                LD R3, ASCII_ZERO       ; R3 is now the Ascii Zero

                JSR RECURSE             ; Jump to the recurse method
                HALT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This method gets a (possibly multi-digit) number of times the recursive method should recurse.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RECUR_CT        ADD R6, R6, #1          ; Incremenent stack pointer
                STR R7, R6, #0          ; Store return address
                AND R2, R2, #0          ; Zero out R2, this is the length of Recur ct input
                AND R3, R3, #0          ; Zero out R3, this is the pointer in RECUR_CT_STORE
                LEA R3, RECUR_CT_STORE  ; Load the first mem address of RECUR_CT_STORE
RECUR_CT_LOOP   GETC                    ; Listen for character
                LD R1, NEG_NEWLINE      ; Load negative newline for termination check
                ADD R1, R0, R1          ; Add input to NEG_NEWLINE
                BRz PARSE_RECUR_CT      ; If new line, terminate input.
                LD R1, NEG_2            ; Load the recurse count limit
                ADD R1, R1, R2          ; Calculate the difference between length and limit
                BRz RECUR_CT_LOOP       ; If at limit, keep looping until newline
                PUTC                    ; Echo R0            
                STR R0, R3, #0          ; Store inputted char from R0 to next RECUR_CT_STORE location
                ADD R2, R2, #1          ; Increment the RECUR_CT_STORE index
                ADD R3, R3, #1          ; Increment the input length
                ADD R5, R3, R2          ; Calculate the difference between length and limit
                BRnzp RECUR_CT_LOOP       ; Continue listening for input
PARSE_RECUR_CT  AND R2, R2, #0          ; Zero out R2. This will be used to check values
                LEA R3, RECUR_CT_STORE  ; Load the address of the recurse count store
                ADD R3, R3, #1          ; Go to the second digit in the variable
                LDR R4, R3, #0          ; Load this character/digit into R4
                LD R1, ASCII_ZERO   ; Negative ascii zero
                ADD R2, R1, R4          ; R4 has been converted from ascii to integer
                BRzp MULTI_DIGIT        ; If this is a number, then we have a multi-digit number
                ADD R3, R3, #-1         ; If not, Move back to the first location of RECUR_CT_STORE
                LDR R4, R3, #0          ; Load the data at that location
                ST R4, RECUR_CT_INT     ; Assuming the first character was given an integer input
                                        ; Store the first character in the input storage to RECUR_CT_INT.
                BRnzp RECUR_CT_RET      ; Just return
MULTI_DIGIT     AND R2, R2, #0          ; Zero out R2. This will now be used to accumulate the values
                ADD R2, R2, R4          ; Add R4 to R2. (The second digit in RECUR_CTSTORE)
                ADD R3, R3, #-1         ; Move back to the first location of RECUR_CT_STORE
                LDR R4, R3, #0          ; Load the data at that location
                LD R1, ASCII_ZERO        ; Load Negative ascii zero
                ADD R4, R4, R1          ; Convert R4 from ascii to integer number
MULTI_DIGIT_LP  ADD R2, R2, #10         ; Add 10 to the accumulated value
                ADD R4, R4, #-1         ; Subtract 1 from R4
                BRp MULTI_DIGIT_LP      ; Keep looping until the this 10s place number is not positive
                ST R2, RECUR_CT_INT     ; Store R2 in the recurse count variable
RECUR_CT_RET    LDR R7, R6, #0          ; Pop the return address off RET_STACK
                ADD R6, R6, #-1         ; Decrement stack pointer
                RET                     ; Return to main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This method recurses and prints a message the specified number of times.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
RECURSE         ADD R6, R6, #1          ; Incremenent stack pointer
                STR R7, R6, #0          ; Store return address
                LEA R0, RECURSE_MSG     ; Load the recurse message
                PUTS                    ; Print recurse message
                ADD R1, R1, #1          ; Increment number of times recursed
                AND R0, R0, #0          ; Clear R0
                ADD R0, R1, #0          ; Set R0 to R1
                ADD R0, R3, #0          ; Add Ascii Zero to R0
                PUTC                    ; Print the number of times recursed
                LEA R0, NEWLINE         ; Load the newline string
                PUTS                    ; Print newline
                GETC                    ; Pause
                ADD R4, R2, R1          ; Add the negative of the number of times to recurse.
                BRz RECURSE_RET         ; When this is zero it's time to stop recursing
                JSR RECURSE             ; Go deeper!
RECURSE_RET     LEA R0, DECURSE_MSG     ; Load decurse message (Is that a word?)
                PUTS                    ; Print it
                AND R0, R0, #0          ; Clear R0
                ADD R0, R1, #0          ; Set R0 to R1
                ADD R0, R3, #0          ; Add Ascii Zero to R0
                PUTC                    ; Print the recurse we're returning from
                LEA R0, NEWLINE         ; Load the newline string
                PUTS                    ; Print newline
                GETC                    ; Pause
                ADD R1, R1, #-1         ; Decrement the recurse counter
                LDR R7, R6, #0          ; Pop the return address off RET_STACK
                ADD R6, R6, #-1         ; Decrement stack pointer
                RET                     ; Return return return

; Data

RECUR_CT_PROMPT .STRINGZ        "\nHow many times would you like to recurse? (1-25)"
RECURSE_MSG     .STRINGZ        "This is recurse number: "
DECURSE_MSG     .STRINGZ        "Returning from recurse number: "
NEWLINE         .STRINGZ        " (press Enter)\n"

RECUR_CT_STORE  .BLKW 2                         ; Stores the recurse count input
RECUR_CT_INT    .BLKW 1                         ; Store the integer of the recurse count

ASCII_ZERO      .FILL           #48             ; Ascii Zero
NEG_2           .FILL           #-2             ; A negative 2
NEG_NEWLINE     .FILL           xFFF6           ; A negative newline
NEG_ASCII_ZERO  .FILL           #-48            ; A negative 48.
NOTHING_BURGER  .FILL           x0000           ; A big nothing buger.
                                                ; This is where we will insert push/pop instructions 
                                                ; after assembling with the LC3 assembler.
                .END