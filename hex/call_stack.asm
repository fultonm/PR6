; A program for testing the push/pop unused opcode
; Michael Fulton
; TCSS 372


; Program
                .ORIG x3000             ; Start at x3000
                LEA R6, RET_STACK       ; Load the address of the first position in the stack
                ADD R6, R6, #-1         ; Set stack pointer to negative 1. We increment before push

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This is like the "main" method
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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
                LEA R4, RECUR_CT_STORE  ; Load the first mem address of RECUR_CT_STORE
RECUR_CT_LOOP   GETC                    ; Listen for character
                ADD R2, R2, #1          ; Increment the input length
                LD R1, NEG_NEWLINE      ; Load negative newline for termination check
                ADD R1, R0, R1          ; Add input to NEG_NEWLINE
                BRz PARSE_RECUR_CT      ; If new line, terminate input.
                LD R1, NEG_2            ; Load the recurse count limit
                ADD R1, R1, R2          ; Calculate the difference between length and limit
                BRz RECUR_CT_LOOP       ; If at limit, keep looping until newline
                PUTC                    ; Echo R0
                STR R0, R4, #0          ; Store inputted char from R0 to next RECUR_CT_STORE location
                ADD R4, R4, #1          ; Increment the RECUR_CT_STORE index
                BRnzp RECUR_CT_LOOP     ; Continue listening for input
PARSE_RECUR_CT  LEA R4, RECUR_CT_STORE  ; Load the first mem address of RECUR_CT_STORE
                AND R2, R2, #0          ; Zero out R2. This will be used to check values
                LDR R2, R4, #1          ; Load the second character/digit (offset RECUR_CT_STORE by one) into R4
                LD R1, NEG_ASCII_ZERO   ; Negative ascii zero
                ADD R2, R2, R1          ; R2 has been converted from ascii to integer
                BRzp MULTI_DIGIT        ; If this is not a null value, then assume a multi-digit number
                LDR R2, R4, #0          ; Load the character/digit at the first location in the RECUR_CT_STORE
                ADD R2, R2, R1          ; R2 has been converted from ascii to integers
                ST R2, RECUR_CT_INT     ; Assuming the first character was given an integer input
                                        ; Store the first character in the input storage to RECUR_CT_INT.
                BRnzp RECUR_CT_RET      ; Just return
MULTI_DIGIT     AND R3, R3, #0          ; Zero out R3. This will be used to hold the 10s place
                LDR R2, R4, #1          ; Put the second ascii character of RECUR_CT_STORE in R2
                LD R1, NEG_ASCII_ZERO   ; Negative ascii zero
                ADD R2, R2, R1          ; R2 has been converted from ascii to integer
                LDR R3, R4, #0          ; Load the ascii character in the 10s place of RECUR_CT_STORE
                ADD R3, R3, R1          ; Convert R4 from ascii to integer number
MULTI_DIGIT_LP  ADD R2, R2, #10         ; Add 10 to the accumulated value
                ADD R3, R3, #-1         ; Subtract 1 from R4
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
                ADD R0, R1, #0          ; Set R0 to R1
                ADD R0, R0, R3          ; Add Ascii Zero to R0
                PUTC                    ; Print the number of times recursed
                LEA R0, ENTER_PROMPT         ; Load the newline string
                PUTS                    ; Print newline
                GETC                    ; Pause
                ADD R4, R2, R1          ; Add the negative of the number of times to recurse.
                BRz RECURSE_RET         ; When this is zero it's time to stop recursing
                JSR RECURSE             ; Go deeper!
RECURSE_RET     LEA R0, DECURSE_MSG     ; Load decurse message (Is that a word?)
                PUTS                    ; Print it
                ADD R0, R1, #0          ; Set R0 to R1
                ADD R0, R0, R3          ; Add Ascii Zero to R0
                PUTC                    ; Print the recurse we're returning from
                LEA R0, ENTER_PROMPT    ; Load the newline string
                PUTS                    ; Print newline
                GETC                    ; Pause
                ADD R1, R1, #-1         ; Decrement the recurse counter
                LDR R7, R6, #0          ; Pop the return address off RET_STACK
                ADD R6, R6, #-1         ; Decrement stack pointer
                RET                     ; Return return return

; Data

RECUR_CT_PROMPT .STRINGZ        "\nHow many times would you like to recurse? (1-25)"
RECURSE_MSG     .STRINGZ        "\nThis is recurse number: "
DECURSE_MSG     .STRINGZ        "\nReturning from recurse number: "
ENTER_PROMPT    .STRINGZ        " (press Enter)"

RECUR_CT_STORE  .BLKW 2                         ; Stores the recurse count input
RECUR_CT_INT    .BLKW 1                         ; Store the integer of the recurse count

ASCII_ZERO      .FILL           #48             ; Ascii Zero
NEG_2           .FILL           #-2             ; A negative 2
NEG_NEWLINE     .FILL           xFFF6           ; A negative newline
NEG_ASCII_ZERO  .FILL           #-48            ; A negative 48.
NOTHING_BURGER  .FILL           x0000           ; A big nothing buger.
                                                ; This is where we will insert push/pop instructions 
                                                ; after assembling with the LC3 assembler.
RET_STACK       .BLKW 12                         ; Keep a stack of return addresses. This is just for testing before putting in push/pop instruction

                .END