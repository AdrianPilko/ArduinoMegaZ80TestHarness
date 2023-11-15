    .org $0000      ;; mainly testing the srec read in
    jp $0010
    
    .org $0010      
    ld   hl,$03f0
    ld   sp, hl  
    
    ld b, $5
innerLoop:        
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop    ;; BUG somewhere in setup (arduino or wiring) gets here then jumps to $40???
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop    
    ;ld a, 1
    ;out (1), a
    ;ld a, 0
    ;out (1), a
    ;ld a, 1
    ;out (1), a    
    ;ld a, 0
    ;out (1), a    
    ;;djnz innerLoop ;;; LATEST theory now is djnz not working!
    jp innerLoop
    halt      
    .org $0040       ; make sure it halts, was seeing an error where it jumps pc to $40 ??
    halt
    halt
    halt
#END

