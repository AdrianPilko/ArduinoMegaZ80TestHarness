;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
;;; does not set stack pointer, previous tests should not working properly yet
#define ROM_SIZE $0200
#define SIZE_OF_SYSTEM_VARIABLES $0004
#define STACK_SIZE_BYTES $0008
#define STACK_BOTTOM $0010+ROM_SIZE+SIZE_OF_SYSTEM_VARIABLES
#define RAM_SIZE $0080   ;;; this will be checked
    
    ;;; initialise LEDs on port zero and one to be off
    .org $0
    ld  sp , STACK_BOTTOM 
    ld hl, STACK_BOTTOM   ; start of ram at $200 for small ram in emulator
    ld e, $55       ; fill all memory with $55 = 1010101
memFillAndCheck:
    ld a, e   
    ld (hl), a
    ld a, (hl)
    cp e
    jp nz, endMemCheck
    inc hl  
    jp memFillAndCheck
    ; only gets here if it didn't find top of ram?!
    halt
     
endMemCheck:
    ld a, 1
    out (1), a     ; output led port
    ;; write the value of hl to memory location $201
    ld (RAM_MAX_VAR), hl

    ;; test the stack
    ld b, STACK_SIZE_BYTES    
stackPushCheckLoop:         
    ld hl, $ffff
    push hl
    djnz stackPushCheckLoop
    
    ld b, STACK_SIZE_BYTES
stackCheckLoop:    
    pop hl    
    ld a, $ff       
    cp h
    jp nz, postFail
    cp l
    jp nz, postFail    
    djnz stackCheckLoop
    
    ld a, 1
    out (0), a     ; output 1 to port zero (I have green connected)
    ;;; also write post result to POST_RESULT $BEEF = pass $FBAD = fail
    ;; as in memory it's low byte high byte, reverse $EFBE = pass $ADFB = fail
    ld hl, $EFBE 
    ld (POST_RESULT), hl
    jp runMonitor    

runMonitor
    ; not yet implemented,   
    xor a
    out (0), a
    out (1), a
    out (2), a
    
    ld a, 1
    out (0), a     ; output 1 to port zero (I have green connected)
    xor a
    out (0), a     ; output 1 to port zero (I have green connected)
    jp runMonitor
    halt
    
postFail
    ld a, 1
    out (1), a     ; output led port - red
    ld hl, $ADFB
    ld (POST_RESULT), hl    
    halt
    .org ROM_SIZE  
RAM_MAX_VAR:
    .dw $ffff
POST_RESULT:    
    .dw $ffff    
#END

