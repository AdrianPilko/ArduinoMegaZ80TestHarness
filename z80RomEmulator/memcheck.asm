;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
;;; does not set stack pointer, previous tests should not working properly yet
#define ROM_SIZE $0200
#define STACK_BOTTOM $0210
#define RAM_SIZE $0032
    
    ;;; initialise LEDs on port zero and one to be off
    .org 0
    ld  sp , STACK_BOTTOM 
    ld hl, STACK_BOTTOM+ROM_SIZE   ; start of ram at $200 for small ram in emulator
    ld e, $55       ; fill all memory with $55 = 1010101
memFillAndCheck:       
    ld a, e   
    ld (hl), a
    ld a, (hl)
    cp e
    inc hl   
    jp nz, failedMemCheckOrTopRam
    
    jp memFillAndCheck
    ; only gets here if it didn't find top of ram?!
    halt
     
    ;;; if we failed memory check then flash the led on port zero
failedMemCheckOrTopRam:
    ld a, 1
    out (2), a     ; output 1 to port zero (I have red led connected)
    ;; write the value of hl to memory location $201
    ld (RAM_LIMIT_VAR), hl
    halt
    .org ROM_SIZE+1
RAM_LIMIT_VAR:
    .dw $0000
#END

