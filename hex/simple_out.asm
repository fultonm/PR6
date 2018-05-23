; A small program for testing multi-line output
; Michael Fulton
; TCSS 372


; Program
                .ORIG x3000             ; Start at x3000
                LEA R0, OUT_LARGE       ; Load up the large message
                PUTS                    ; Print the string
                GETC                    ; Pause
                LEA R0, OUT_MEDIUM      ; Load the medium message
                PUTS                    ;
                GETC                    ;
                LEA R0, OUT_SMALL       ;
                PUTS                    ;
                GETC                    ;
                LEA R0, OUT_MEDIUM      ; Load the medium message
                PUTS                    ;
                GETC                    ;
                LEA R0, OUT_LARGE       ; Load the large message
                PUTS                    ;
                GETC                    ;
                HALT

; Data

OUT_LARGE       .STRINGZ        "\nThis is a large output"
OUT_MEDIUM      .STRINGZ        "\nMedium output"
OUT_SMALL       .STRINGZ        "\nSmall"

                .END