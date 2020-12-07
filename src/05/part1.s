;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

main_start:
  ; Initialize the stack pointer to some high value
  MOVIL 0xfffc sp
  MOVIH 0x0000 sp

  ; Local variables
  ; r5 - loop counter
  ; r6 - character
  ; r7 - constant 0x0a (\n)
  ; r8 - constant 0x00 (EOF)
  ; r2 - result
  MOVI 0 r5
  MOVI 0x0a r7
  MOVI 0 r8
  MOVI 0 r2

  for_lines:
    LD8 r5 0 r6

    ; Check if the char is a zero
    CMP r6 r8
    JMPZ main_end

    ; Call the get seat number function
    MOV r5 r1
    CALL get_seat_number
    MOV r1 r5

    ; Is it bigger than our best?
    CMP r0 r2
    JMPGZ id_is_bigger
    JMP id_is_smaller

    id_is_bigger:
      MOV r0 r2
    id_is_smaller:

    ; Continue to next iteration
    JMP for_lines

; End of program
main_end:
  MOV r2 r0
  MOVI 0 r1
  MOVI 0 r2
  HLT

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
