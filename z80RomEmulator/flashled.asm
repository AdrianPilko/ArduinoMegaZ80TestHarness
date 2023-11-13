    .org 0          
    ld   hl,$03f0
    ex   (sp),hl  
    
    ld b, $ff
outerLoop:      
    push bc 
    ld a, 1  
    out (0), a
    
    ld b, $2
innerLoop:        
    call setLedOn_PORT1  
    nop    
    call setLedOff_PORT1
    nop
    djnz innerLoop

    xor a
    out (0), a
    
    pop bc
    djnz outerLoop
    halt      
    
setLedOn_PORT1:
    ld a, 1    
    out (1), a
    ret
setLedOff_PORT1:
    ld a, 0
    out (1), a
    ret    
#END

