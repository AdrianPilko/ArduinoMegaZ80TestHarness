    .org 0          
    ld sp, $00ff
    ld b, $ff
outerLoop:      
    push bc 
    ld b, $f

    ld a, 1  
    out (0), a
    
innerLoop:    
    xor a
    out (1), a
    ld a, 1  
    out (1), a
    
    djnz innerLoop

    xor a
    out (0), a
    
    pop bc
    djnz outerLoop
    halt      
    
setLedOn_PORTc:
    ld a, 1
    ;out (c), a    
    out (1), a
    ret
setLedOff_PORTc:
    xor a
    out (1), a
    ;out (c), a    
    ret    
#END

