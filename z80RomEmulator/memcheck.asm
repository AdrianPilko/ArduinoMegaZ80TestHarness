;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
;;; does not set stack pointer, previous tests should not working properly yet
    
    ;;; initialise LEDs on port zero and one to be off
    .org 0
    ld   hl,$03f0
    ex   (sp),hl      
    xor a
    out (0), a
    out (1), a
        
    ld hl, $0400    ; start of ram at $400 
    ld b, $02      ; 255 bytes to check (with more mem need nested loop)

memFill:
    ld a, 1     ;; flash led on port 1
    out (1), a
    ld a, 0
    out (1), a    
    
    ld a, $55   ; fill all memory with $55 = 1010101
    ld (hl), a
    inc hl   
    djnz memFill

    ld a, 1         ; light up led on port 0
    out (0), a
    
    ld hl, $0400    ; start of ram at $400 but need to allow 32bytes for stack
    ld b, $02      ; 255 bytes to check (with more mem need nested loop)

memReadBack:    ;; now check that the value writtin can be read back  
    ld a, 1     ;; flash led on port 1
    out (1), a
    ld a, 0
    out (1), a   
    
    ld e, $55
    ld a, (hl)   
    cp e
    jp z, failedMemCheck
    djnz memReadBack
   
    ld a, 1
    out (0), a
    ld a, 1
    out (1), a    
    halt
    ;;; if we failed memory check then flash the led on port zero
failedMemCheck:
    jp failedMemCheck
    halt
#END

