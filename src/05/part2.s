main_start:
  ; Initialize the stack pointer to some high value
  MOVIL 0xfffc sp
  MOVIH 0x0000 sp

  ; Compute seat presence
  MOVI 0 r0
  MOVI 0x4000 r1
  CALL get_seat_presence

  ; Iterate over the presence array
  find_empty_seat:
    ; Loop variables
    ; r5 - loop counter
    ; r6 - constant - loop end
    ; r7 - base of presence array
    ; r8 - array pointer
    ; r9 - zero
    ; r10 - presence array value
    MOVI 1 r5
    MOVI 0x3ff r6
    MOVI 0x4000 r7
    MOVI 0 r9

  find_empty_seat_loop:
    CMP r5 r6
    JMPZ main_end

    ; Seat validity check
    ADD r5 r5 r8
    ADD r8 r8 r8
    ADD r7 r8 r8
    LD32 r8 0 r10
    CMP r9 r10
    JMPNZ if_zero_end

    if_zero:
      LD32 r8 4 r10
      CMP r9 r10
      JMPZ if_zero_end
      LD32 r8 -4 r10
      CMP r9 r10
      JMPZ if_zero_end
      JMP main_end
    if_zero_end:

    ; Continue to next iteration
    ADDI r5 1 r5
    JMP find_empty_seat_loop

; End of program, output results
main_end:
  MOV r5 r0
  HLT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function to get array of booleans as to
; whether a given seat is present or not
; parameters
; r0 - pointer into char array
; r1 - pointer into presence array
get_seat_presence:

  ; First, clear the presence array
  zero_presence_array:
    ; Loop variables
    ; r5 - loop counter
    ; r6 - constant - loop end
    ; r7 - base of presence array
    ; r8 - array pointer
    ; r9 - constant - zero
    MOVI 0 r5
    MOVI 0x400 r6
    MOV r1 r7
    MOVI 0 r9

  zero_presence_array_loop:
    CMP r5 r6
    JMPZ for_lines

    ; Store a zero
    ADD r5 r5 r8
    ADD r8 r8 r8
    ADD r7 r8 r8
    ST32 r8 0 r9

    ; Continue to next iteration
    ADDI r5 1 r5
    JMP zero_presence_array_loop

  for_lines:
    ; Local variables
    ; r5 - loop counter
    ; r6 - character
    ; r7 - constant 0x0a (\n)
    ; r8 - constant 0x00 (EOF)
    ; r9 - presence array base
    ; r10 - constant - 1
    MOV r0 r5
    MOVI 0x0a r7
    MOVI 0 r8
    MOV r1 r9
    MOVI 1 r10

  for_lines_loop:
    LD8 r5 0 r6

    ; Check if the char is a zero
    CMP r6 r8
    JMPZ get_seat_presence_end

    ; Call the get seat number function
    MOV r5 r1
    CALL get_seat_number
    MOV r1 r5

    ; Update the presence array appropriately
    ADD r0 r0 r0
    ADD r0 r0 r0
    ADD r0 r9 r0
    ST32 r0 0 r10

    ; Continue to next iteration
    JMP for_lines_loop

get_seat_presence_end:
  RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Function to get the seat number
; parameters
; r1 - pointer into character array
; return values
; r0 - seat number
; r1 - pointer into char array once finished
get_seat_number:

  ; Local variables
  ; r5 - loop counter
  ; r6 - character
  ; r7 - constant 0x0a (\n)
  ; r8 - constant 0x42 (B)
  ; r9 - constant 0x46 (F)
  ; r10 - constant 0x52 (R)
  ; r11 - constant 0x4c (L)
  ; r12 - result
  MOV r1 r5
  MOVI 0x0a r7
  MOVI 0x42 r8
  MOVI 0x46 r9
  MOVI 0x52 r10
  MOVI 0x4c r11
  MOVI 0 r12

  ; Loop until we find a newline character
  for_chars_in_line:
    LD8 r5 0 r6
    ADDI r5 1 r5

    ; Check if the char is a newline
    CMP r6 r7
    JMPZ get_seat_number_end

    ; Check if the char is B
    CMP r6 r8
    JMPZ char_represents_one
    CMP r6 r9
    JMPZ char_represents_zero
    CMP r6 r10
    JMPZ char_represents_one
    CMP r6 r11
    JMPZ char_represents_zero

    ; Unrecognized character
    JMP error

  char_represents_one:
    ADD r12 r12 r12
    ADDI r12 1 r12
    JMP for_chars_in_line

  char_represents_zero:
    ADD r12 r12 r12
    JMP for_chars_in_line

; Return the result in r0
get_seat_number_end:
  MOV r12 r0
  MOV r5 r1
  RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Jump to this if something bad happens
; leaves 0x1badf00d in r0 and halts
error:
  MOVIH 0x1bad r0
  MOVIL 0xf00d r0
  HLT
