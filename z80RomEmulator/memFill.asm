    ;;; initialise LEDs on port zero and one to be off
    .org 0      
    ld hl, $7f    
    ld b, $ff      
memFill:
    ld a, $55       ; fill all memory with hex $55 = 1010101    
    ld (hl), a     ; write a to memory location pointed to by hl
    inc hl         ; move hl to next memory location
    djnz memFill
    halt
#END

