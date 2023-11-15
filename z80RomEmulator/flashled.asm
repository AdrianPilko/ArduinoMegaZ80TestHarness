    .org $0000  
    ld   hl,$03f0
    ld   sp, hl  
    
    ld b, $255
innerLoop:        
    ld a, 1
    out (1), a
    ld a, 0
    out (1), a
    ld a, 1
    out (1), a    
    ld a, 0
    out (1), a    
    djnz innerLoop 
    halt      
#END

