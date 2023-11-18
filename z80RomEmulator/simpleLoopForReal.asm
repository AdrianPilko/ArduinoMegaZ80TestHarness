    .org 0
    ld a,$00
start:          
    out ($00), a   ; 11 clock cycles 
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop    
    jp start           ; 10 clock cycles
    
    ;; assuming we have a 1MHz clock then 7+ 7*4 + 2* 11 + 10
#END    
