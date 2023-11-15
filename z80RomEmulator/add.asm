    .org 0
    ld a, $10
    ld b, $10    
    add a, b
    ld hl, $0201
    ld (hl), a
    halt
    ret   
#END    