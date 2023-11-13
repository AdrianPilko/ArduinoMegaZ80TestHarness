    .org 0
    ; theory is that you have to set the SP by the following, not ld sp, $400
    ld   hl,$0400
    ex   (sp),hl    
        
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
    call doNopAndRet
    ld a, 1  
    out (1), a    
    djnz innerLoop   
    pop bc
    djnz outerLoop         
    
doNopAndRet:   ; subrouting to test subroutines work!
    nop
    ret
    .org $44
    halt
    halt
    halt
    halt
    halt
    halt
    halt
#END

