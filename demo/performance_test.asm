; Shane Menzies - HW 11

.ORIG x3000

; .text
START
    ; Jump down to main
    BRnzp MAIN

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

    ; R0 = used for fibonacci subroutine
    ; R1 = current place in data
    ; R2 = current n
    ; R3 = negative of max n

    LEA R0, START_MESSAGE
    TRAP x22

    LEA R1, DATA_START
    AND R2, R2, x0
    LD R3, MAX_TERM

MAIN_LOOP
    ADD R0, R2, x0

    ; Calculate next term
    JSR FIBONACCI
    STR R0, R1, x0

    ; Increment and loop
    ADD R2, R2, x1
    ADD R1, R1, x1
    ADD R0, R2, R3
    BRnz MAIN_LOOP

    LEA R0, END_MESSAGE
    TRAP x22
    HALT
; .data

MAX_TERM
    .FILL x-1c

STACK_TEMP
    .FILL x0

RECURSIVE_TEMP
    .FILL x0

SAVED_CALLER_STACK
    .FILL x0

OVERFLOW_ERROR
    .STRINGZ "ERR:full stk\n"

UNDERFLOW_ERROR
    .STRINGZ "ERR:empty stk\n"

START_MESSAGE
    .STRINGZ "Starting test...\n"
END_MESSAGE
    .STRINGZ "Test Complete.\n"

STACK_TOP
    .BLKW x80
STACK_BOTTOM
    .FILL x0

DATA_START
    .BLKW x1000

.END