    .org 0
  
    ld sp, $00ff
    ld b, $ff
outerLoop:      
    ld a, 1  
    out (0), a    
    nop
    nop    
    ld a, 0
    out (0), a

    push bc 
    ld b, $f    
innerLoop:    
    ld a, 0
    out (1), a
    nop
    nop    
    ld a, 1  
    out (1), a    
    djnz innerLoop   
    pop bc
    djnz outerLoop         
#END

