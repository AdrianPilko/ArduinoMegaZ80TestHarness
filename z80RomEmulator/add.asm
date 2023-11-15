    .org 0
    ld a, $10
    ld b, $10    
    add a, b
    ld hl, $0201 ;; load hl with place to store the result
    ld (hl), a   ;; store the result of the addition into RAM
    halt  
    ret   ;; never gets here
#END    
