; Encrypts and decrypts messages
; Michael Fulton
; TCSS 371 Winter 2018


; Program
                .ORIG x3000             ; Start at x3000

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GET_INPUTS      JSR GET_OP              ; Get the Operation
                JSR GET_KEY             ; Get the encryption key
                JSR GET_MSG             ; Get the message encrypt/decrypt                
                LD R1, OP_STORE         ; Load value of OP_STORE
                LD R2, NEG_D            ; Load negative ASCII D
                ADD R1, R1, R2          ; Add negative D
                BRz GO_DECRYPT          ; Decrypt if a D was entered
                JSR ENCRYPT             ; Otherwise go to ENCRYPT
                BRnzp PRINT_MSG         ; Don't decrypt when returning
GO_DECRYPT      JSR DECRYPT             ; 
PRINT_MSG       LEA R0, MSG_STORE       ; Load beginning of the modified message
                PUTS                    ; Print it
                HALT                    ; Halt

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GET_OP          ST R7, RET_ADDR         ; Store return address
                LEA R0, OP_PROMPT       ; Load the beginning of Operation Prompt message
                PUTS                    ; Print the Operation Prompt
                GETC                    ; Listen for char
                PUTC                    ; Echo R0
                LEA R1, OP_STORE        ;
                STR R0, R1, #0          ; Store inputted char from R0 to OP_STORE
                LD R7, RET_ADDR         ;
                RET                     ;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GET_KEY         ST R7, RET_ADDR         ; Store return address
                LEA R0, KEY_PROMPT      ; Load the beginning of Key Prompt message
                PUTS                    ; Print the Key Prompt
                GETC                    ; Listen for char
                PUTC                    ; Echo R0
                LD R1, NEG_ASCII_NUM    ; Load negative 30 to convert ascii to integer
                ADD R0, R0, R1          ; Subtract 30
                ST R0, KEY_STORE        ; Store the encryption key from R0 to OP_STORE
                LD R7, RET_ADDR         ;
                RET                     ;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
GET_MSG         ST R7, RET_ADDR         ; Store return address
                LEA R0, MSG_PROMPT      ; Load the beginning of Message Prompt message
                PUTS                    ; Print the Message Prompt
                LEA R1, MSG_STORE       ; Load where to store the message string
                AND R2, R2, #0          ; We will use R2 to store the string length counter
                LD R3, NEG_19           ; And will use R3 for string length limit (20 + null term)
                LD R4, NEG_NEWLINE      ; R4 will hold a negative newline for termination check
GET_MSG_STR     GETC                    ; Listen for character
                ADD R5, R0, R4          ; Add input to NEG_NEWLINE
                BRz GET_MSG_TERM        ; If new line, terminate string.
                ADD R5, R3, R2          ; Calculate the difference between length and limit
                BRz GET_MSG_STR         ; If at limit, keep looping until newline
                PUTC                    ; Echo R0            
                STR R0, R1, #0          ; Store inputted char from R0 to next MSG_STORE location
                ADD R1, R1, #1          ; Increment the string index
                ADD R2, R2, #1          ; Increment String length
                ADD R5, R3, R2          ; Calculate the difference between length and limit
                BRnzp GET_MSG_STR       ; Continue listening for chars
GET_MSG_TERM    PUTC                    ; Print the new line that got us here.
                AND R0, R0, #0          ; Set R0 to null terminator
                STR R0, R1, #0          ; Append null terminator
                LD R7, RET_ADDR         ; Return
                RET                     ;  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DECRYPT         ST R7, RET_ADDR         ; Store return address
                LEA R1, MSG_STORE       ; Load the beginning of the message to decrypt
                LD R5, KEY_STORE        ; Load the encryption key.
                NOT R5, R5              ; Calculate the negative of the encryption key
                ADD R5, R5, #1          ; Two's comp
DECRYPT_EL      LDR R2, R1, #0          ; Load the element to decrypt
                ADD R6, R2, #0          ; Check for null terminator
                BRz DECRYPT_RETURN      ; If null term, stop decryption
                JSR TOGL_LOW_BIT        ; Toggle the lowest bit
                ADD R2, R2, R5          ; Add the encryption key
                STR R2, R1, #0          ; Replace message char with encrypted char
                ADD R1, R1, #1          ; Increment the string index
                BRnzp ENCRYPT_EL        ; Continue decryption
DECRYPT_RETURN  LD R7, RET_ADDR         ;
                RET                     ;      

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ENCRYPT         ST R7, RET_ADDR         ; Store return address
                LEA R1, MSG_STORE       ; Load the beginning of the message to encrypt
                LD R5, KEY_STORE        ; Load the encryption key.
ENCRYPT_EL      LDR R2, R1, #0          ; Load the element to encrypt
                ADD R6, R2, #0          ; Check for null terminator
                BRz ENCRYPT_RETURN      ; If null term, stop encryption
                JSR TOGL_LOW_BIT        ; Toggle the lowest bit
                ADD R2, R2, R5          ; Add the encryption key
                STR R2, R1, #0          ; Replace message char with encrypted char
                ADD R1, R1, #1          ; Increment the string index
                BRnzp ENCRYPT_EL        ; Continue encryption
ENCRYPT_RETURN  LD R7, RET_ADDR         ;
                RET                     ;      

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
TOGL_LOW_BIT    ST R7, RET_ADDR_2       ; Store return address
                LD R4, TOGL_MASK        ; Load the toggle mask to apply to R2 bits
                AND R3, R2, R4          ; AND the value with the mask.
                BRp SUB_ONE             ; If the result is a 1, we jump to subtract one
                ADD R2, R2, #1          ; If not let's ADD 1 now
                LD R7, RET_ADDR_2       ; And return
                RET                     ; 
SUB_ONE         ADD R2, R2, #-1         ; If the result is a 0, we'll add 1 to toggle
                LD R7, RET_ADDR_2       ; And return
                RET                     ; 



; Data
OP_STORE        .BLKW 1
KEY_STORE       .BLKW 1             
MSG_STORE       .BLKW 21
NEG_D           .FILL               xFFBC
NEG_NEWLINE     .FILL               xFFF6
NEG_19          .FILL               #-19
NEG_ASCII_NUM   .FILL               #-48
TOGL_MASK       .FILL               x0001
RET_ADDR        .BLKW 1
RET_ADDR_2      .BLKW 1
OP_PROMPT       .STRINGZ            "\nType (E)ncrypt/(D)ecrypt: "        
KEY_PROMPT      .STRINGZ            "\nEnter encryption key(1-9): "
MSG_PROMPT      .STRINGZ            "\nEnter message (19 char limit): "
NEWLINE         .STRINGZ            "\n"

                .END