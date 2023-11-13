    .org 0
    ; theory is that you have to set the SP by the following, not ld sp, $400
    ld   hl,$0400
    ex   (sp),hl    
    
    ld hl, $0055   ; binary 01010101
    push hl    
    ld hl, $aa     ; binary 10101010
    push hl
    
    pop bc
    pop bc
    call doNopAndRet
    halt
    
doNopAndRet:   ; subrouting to test subroutines work!
               ;; only started working when used     
               ;ld hl,$0400 then ex (sp),hl  to set sp
                                ; 
    nop
    ret
#END

