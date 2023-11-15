    .org $0010          
    ld   hl,$03f0
    ld   sp, hl  
    
    ld b, $5
innerLoop:        
    ld a, 1    
    out (1), a
    nop
    nop
    nop
    nop        
    ld a, 0
    ld a, 0
    ld a, 0
    out (1), a   ;; seams to fail here in arduino code and jump to 0x40???
    out (1), a
    out (1), a    
    nop
    nop
    nop
    nop        
    djnz innerLoop
    halt      
    .org $0040       ; make sure it halts, was seeing an error where it jumps pc to $40 ??
    halt
    halt
    halt
#END

