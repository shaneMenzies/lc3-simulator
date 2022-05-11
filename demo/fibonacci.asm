; Shane Menzies - HW 11

.ORIG x3000

; .text
START
    ; Jump down to main
    BRnzp MAIN

; Convert ascii digits to integer value, returned in R0
ATOI
    ; R0 = null-terminated string to convert (by address)
    ; R1 = used for recursion count (saved)
    ; R6 = Stack Pointer
    ; R7 = Return address (saved)

    ; Prepare for recursive section
    ST R0, RECURSIVE_TEMP
    ADD R0, R7, x0
    JSR PUSH
    ADD R0, R1, x0
    JSR PUSH
    LD R0, RECURSIVE_TEMP

    ; Refer to recursive subroutine
    JSR ATOI_R

    ; Restore registers and return
    ST R0, RECURSIVE_TEMP
    JSR POP
    ADD R1, R0, x0
    JSR POP
    ADD R7, R0, x0
    LD R0, RECURSIVE_TEMP
    RET

; Recursive section of ATOI, returns value in R0, and recursion count in R1
ATOI_R
    ; R0 = null-terminated string to convert
    ; R1 = place value (modified)
    ; R2 = character storage (saved)
    ; R3 = scratch register (saved)
    ; R6 = Stack Pointer
    ; R7 = Return address (saved)

    ; Check if end of string
    ST R0, RECURSIVE_TEMP
    LDR R0, R0, x0
    BRnp ATOI_R_CONT
    AND R1, R1, x0
    ADD R1, R1, x1
    RET

ATOI_R_CONT
    ; Save registers
    ADD R0, R7, x0
    JSR PUSH
    ADD R0, R3, x0
    JSR PUSH
    ADD R0, R2, x0
    JSR PUSH
    LD R0, RECURSIVE_TEMP

    ; Get next character
    LDR R2, R0, x0

    ; Convert this character to it's integer value
    LD R3, ASCII_DIGIT_OFFSET
    NOT R3, R3
    ADD R3, R3, x1
    ADD R2, R2, R3

    ; Get result for rest of string
    ADD R0, R0, x1
    JSR ATOI_R

    ; Add adjusted value of this character to rest of string's value
    JSR MULT
    JSR PUSH

    ; Increase place value (multiply by 10)
    AND R0, R0, x0
    ADD R2, R0, 10
    JSR MULT
    ADD R1, R0, x0

    JSR POP
    ST R0, RECURSIVE_TEMP

    ; Restore registers
    JSR POP
    ADD R2, R0, x0
    JSR POP
    ADD R3, R0, x0
    JSR POP
    ADD R7, R0, x0
    LD R0, RECURSIVE_TEMP
    RET

; Convert integer value to ascii digits
ITOA
    ; R0 = Value to convert
    ; R1 = Used during recursion (saved)
    ; R2 = Used during recursion (saved)
    ; R5 = Buffer to place digits in (saved)
    ; R7 = Return address (saved)

    ; Prepare for recursive section
    ST R0, RECURSIVE_TEMP
    ADD R0, R7, x0
    JSR PUSH
    ADD R0, R5, x0
    JSR PUSH
    ADD R0, R2, x0
    JSR PUSH
    ADD R0, R1, x0
    JSR PUSH
    LD R0, RECURSIVE_TEMP
    
    ; Refer to recursive subroutine
    JSR ITOA_R

    ; Add null termination
    AND R1, R1, x0
    STR R1, R5, x0

ITOA_END
    ; Restore registers and return
    ST R0, RECURSIVE_TEMP
    JSR POP
    ADD R1, R0, x0
    JSR POP
    ADD R2, R0, x0
    JSR POP
    ADD R5, R0, x0
    JSR POP
    ADD R7, R0, x0
    LD R0, RECURSIVE_TEMP
    RET

ITOA_R
    ; R0 = Value to operate on
    ; R1 = Used to determine digit (saved)
    ; R5 = Target buffer
    ; R6 = Stack Pointer
    ; R7 = Return address (saved)
 
    ; Save registers
    ST R0, RECURSIVE_TEMP
    ADD R0, R7, x0
    JSR PUSH
    ADD R0, R1, x0
    JSR PUSH
    LD R0, RECURSIVE_TEMP

    ; Determine character by dividing by 10
    AND R1, R1, x0
    ADD R1, R1, 10
    JSR DIV

    ; Remainder left in R1, Result in R0

    ; Start with rest of number left in R0 (if it exists)
    BRz ITOA_R_CONTINUE
    JSR ITOA_R

ITOA_R_CONTINUE
    ; Place this character
    ST R0, RECURSIVE_TEMP
    LD R0, ASCII_DIGIT_OFFSET
    ADD R0, R0, R1
    STR R0, R5, x0
    ADD R5, R5, x1
    LD R0, RECURSIVE_TEMP

    ; Restore registers and return
    ST R0, RECURSIVE_TEMP
    JSR POP
    ADD R1, R0, x0
    JSR POP
    ADD R7, R0, x0
    LD R0, RECURSIVE_TEMP
    RET

; Multiplies two numbers (R1 and R2), adds offset(R0), returns result in R0
MULT
    ; R0 = current sum (initially offset)
    ; R1 = First operand  (saved)
    ; R2 = Second operand (unmodified)
    ; R7 = Return address (unmodified)

    ST R1, STACK_TEMP
    ; Check if R2 is 0
    ADD R2, R2, x0
    BRz MULT_END

MULT_LOOP
    ; Check if R1 is 0
    ADD R1, R1, x0
    BRz MULT_END

    ; Add to sum
    ADD R0, R0, R2 
    ; Decrease R1 and loop again
    ADD R1, R1, x-1
    BRnzp MULT_LOOP

MULT_END
    LD R1, STACK_TEMP
    RET

; Divides two numbers (R0 / R1), Result in R0, Remainder in R1
DIV
    ; R0 = Result
    ; R1 = Remainder
    ; R2 = Used for second operand (saved)
    ; R3 = Scratch Register (saved)
    ; R7 = Return address (saved)

    ; Save registers
    ST R0, RECURSIVE_TEMP
    ADD R0, R7, x0
    JSR PUSH
    ADD R0, R3, x0
    JSR PUSH
    ADD R0, R2, x0
    JSR PUSH

    ADD R2, R1, x0
    LD R1, RECURSIVE_TEMP
    AND R0, R0, x0

    ; Check if second operand is 0
    ADD R2, R2, x0
    BRz DIV_END

    ; Make second operand negative
    NOT R2, R2
    ADD R2, R2, x1

DIV_LOOP
    ; Check if this can be divided again
    ADD R3, R1, R2
    BRn DIV_END

    ; Still some left in R1, continue
    ADD R0, R0, x1
    ADD R1, R3, x0
    BRnzp DIV_LOOP

DIV_END
    ; Restore registers
    ST R0, RECURSIVE_TEMP
    JSR POP
    ADD R2, R0, x0
    JSR POP
    ADD R3, R0, x0
    JSR POP
    ADD R7, R0, x0
    LD R0, RECURSIVE_TEMP
    RET
    
; Calculates nth term of fibonacci sequence (n = R0, result returned in R0)
FIBONACCI
    ; R0 = nth term to calculate
    ; R1 = Used for sum (Saved)
    ; R2 = Tracks current term (Saved)
    ; R6 = Stack Pointer
    ; R7 = Return address (Saved)

    ADD R0, R0, x-1
    ; R0 = (n-1)
    BRp FIB_CONTINUE
    ; (n <= 1), return 1
    AND R0, R0, x0
    ADD R0, R0, x1
    RET

FIB_CONTINUE
    ; Need to save regs on stack, then continue recursively
    ST R0, RECURSIVE_TEMP   
    ADD R0, R7, x0
    JSR PUSH
    ADD R0, R2, x0
    JSR PUSH
    ADD R0, R1, x0
    JSR PUSH
    LD R2, RECURSIVE_TEMP

    ; Compute (n-1)th and place in R1
    ADD R0, R2, x0
    JSR FIBONACCI
    ADD R1, R0, x0

    ; Compute (n-2)th and add to R1
    ADD R0, R2, x-1
    JSR FIBONACCI
    ADD R1, R1, R0

    ; Save result
    ST R1, RECURSIVE_TEMP

    ; Pop saved regs from the stack
    JSR POP
    ADD R1, R0, x0
    JSR POP
    ADD R2, R0, x0
    JSR POP
    ADD R7, R0, x0

    ; Load result and return
    LD R0, RECURSIVE_TEMP
    RET

; Pushes value in R0 onto the stack in R6
PUSH
    ; R0 = Value to push
    ; R1 = Used for testing for overflow (Saved)
    ; R6 = Current Stack Pointer
    ; R7 = Return Address

    ST R1, STACK_TEMP

    ; R1 = -(Stack_top)
    LEA R1, STACK_TOP
    NOT R1, R1
    ADD R1, R1, x1

    ; R1 = (Current_stack - stack_top)
    ADD R1, R6, R1
    BRz PUSH_OVERFLOW
    ; Trigger an error if the stack is full

    ; Now can push to stack
    ADD R6, R6, x-1
    STR R0, R6, x0

    ; Return
    LD R1, STACK_TEMP
    RET

PUSH_OVERFLOW
    LEA R0, OVERFLOW_ERROR
    BRnzp TRIGGER_ERROR

; Pops value from top of stack (R6), into R0
POP
    ; R0 = Value from top of stack
    ; R1 = Used for testing for underflow (Saved)
    ; R6 = Current Stack Pointer
    ; R7 = Return Address

    ST R1, STACK_TEMP

    ; R1 = -(Stack_bottom)
    LEA R1, STACK_BOTTOM
    NOT R1, R1
    ADD R1, R1, x1

    ; R1 = (Current_stack - stack_bottom)
    ADD R1, R6, R1
    BRz POP_UNDERFLOW
    ; Trigger an error if the stack is empty

    ; Now can pop from stack
    LDR R0, R6, x0
    ADD R6, R6, x1

    ; Return
    LD R1, STACK_TEMP
    RET

POP_UNDERFLOW
    LEA R0, UNDERFLOW_ERROR
    BRnzp TRIGGER_ERROR

; Called when an error has occurred, doesn't return
TRIGGER_ERROR
    ; R0 = Pointer to Ascii error message
    TRAP x22
    HALT

; Swaps to program stack and loops through the program
MAIN
    ; Swap stacks
    ST R6, SAVED_CALLER_STACK
    LEA R6, STACK_BOTTOM

MAIN_LOOP
    ; Print prompt
    LEA R0, INPUT_MESSAGE
    TRAP x22

    ; Get string of input
    LEA R0, STRING_BUFFER
    TRAP x40

    ; Process into integer value
    JSR ATOI

    ; Calculate requested term
    JSR FIBONACCI

    ; Turn result back into string
    LEA R5, STRING_BUFFER
    JSR ITOA

    ; Print Result prompt
    LEA R0, OUTPUT_MESSAGE
    TRAP x22
    LEA R0, STRING_BUFFER
    TRAP x22

    ; Loop back to start
    BRnzp MAIN_LOOP

; .data

STACK_TEMP
    .FILL x0

RECURSIVE_TEMP
    .FILL x0

SAVED_CALLER_STACK
    .FILL x0

ASCII_DIGIT_OFFSET
    .FILL x30

ASCII_NEWLINE
    .FILL x0a
ASCII_CARRIAGE_RETURN
    .FILL x0d

OVERFLOW_ERROR
    .STRINGZ "ERROR: Attempted to push onto a full stack!\n"

UNDERFLOW_ERROR
    .STRINGZ "ERROR: Attempted to pop from an empty stack!\n"

STACK_TOP
    .BLKW x50
STACK_BOTTOM
    .FILL x0

STRING_BUFFER
    .BLKW x10
STRING_BUFFER_END
    .FILL x0

INPUT_MESSAGE
    .STRINGZ "\nInput n: "

OUTPUT_MESSAGE
    .STRINGZ "\nResult: "

.END