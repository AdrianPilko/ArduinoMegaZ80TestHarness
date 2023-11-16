;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
;;; does not set stack pointer, previous tests should not working properly yet
#define ROM_SIZE $0200
#define STACK_BOTTOM $0010   ;; offset from start of RAM, not from $0000
#define RAM_SIZE $0020
    
    ;;; initialise LEDs on port zero and one to be off
    .org $0
    ld  sp , STACK_BOTTOM 
    ld hl, STACK_BOTTOM+ROM_SIZE   ; start of ram at $200 for small ram in emulator
    ld e, $55       ; fill all memory with $55 = 1010101
memFillAndCheck:       
    ld a, e   
    ld (hl), a
    ld a, (hl)
    cp e
    jp nz, failedMemCheckOrTopRam
    inc hl  
    jp memFillAndCheck
    ; only gets here if it didn't find top of ram?!
    halt
     
failedMemCheckOrTopRam:
    ld a, 1
    out (2), a     ; output 1 to port zero (I have red led connected)
    ;; write the value of hl to memory location $201
    ld (RAM_MAX_VAR), hl
    halt
    .org ROM_SIZE+1  
RAM_MAX_VAR:
    .dw $ffff
#END

