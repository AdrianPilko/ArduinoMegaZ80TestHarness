;;the lcd port is port zero on the U4 74ls138,
;; but the addresses that are used are connected to A 5 6 and 7, A4 input to pin   is one of the enable pins
;; so lcd port 00000000 00001000
#define lcdPort 
    
    .org 
    ld a,$f0
start:        
    ;ld a,$55           ; 7 clock cycles
    out (lcdPort), a   ; 11 clock cycles
    ;xor a              ; 4 clock cycles
    ;out (lcdPort), a   ; 11 clock cycles
    ;nop                ; all nops are 4 clock cycles
    ;nop
    ;nop
    ;nop
    ;nop
    ;nop    
    jp start           ; 10 clock cycles
    
    ;; assuming we have a 1MHz clock then 7+ 7*4 + 2* 11 + 10
#END    
