;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
;;; does not set stack pointer, previous tests should not working properly yet
    
    ;;; initialise LEDs on port zero and one to be off
    .org 0
    ld  sp , $03f0      
    xor a
    out (0), a
    out (1), a
    out (2), a
;; test led loop
    ld a, 1         ; light up led on port 0
    out (0), a
    ld a, 1         ; light up led on port 1
    out (1), a
    ld a, 1         ; light up led on port 2
    out (2), a    
    ld b, $30 
testLED:    

    djnz testLED
    xor a
    out (0), a    ; now output zero to the led ports to turn of
    out (1), a
    out (2), a    

    ld hl, $0400    ; start of ram at $400 
    ld b, $ff      ; 255 bytes to check (with more mem need nested loop)
memFill:
    ld a, 1     ;; flash led on port 1 (amber)
    out (1), a
    ld a, 0
    out (1), a    
    
    ld a, $55   ; fill all memory with $55 = 1010101
    ld (hl), a
    inc hl   
    djnz memFill

   
    ld hl, $0400    ; start of ram at $400 but need to allow 32bytes for stack
    ld b, $ff      ; 255 bytes to check (with more mem need nested loop)

memReadBack:    ;; now check that the value writtin can be read back  
    ld a, 1     ;; flash led on port 1 (amber)
    out (1), a
    ld a, 0
    out (1), a   
    
    ld e, $55
    ld a, (hl)   
    cp e
    jp nz, failedMemCheck
    djnz memReadBack
   
    ld a, 1
    out (0), a     ; output 1 to port zero (I have green led connected)
    halt
     
    ;;; if we failed memory check then flash the led on port zero
failedMemCheck:
    ld a, 1
    out (2), a     ; output 1 to port zero (I have red led connected)    
    halt
        
#END

