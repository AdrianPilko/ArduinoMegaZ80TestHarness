;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
    
    ;;; initialise LEDs on port zero and one to be off
    .org 0
    ld c, $00
    call setLedOff
    ld c, $01
    call setLedOff    
    
    ld sp, $0400    ;; initialise stack pointer to start of ram
    ld hl, $0420    ; start of ram at $400 but need to allow 32bytes for stack
    ld b, $ff      ; 255 bytes to check (with more mem need nested loop)

memFill:
    ld c, $01      ; turn led on port zero on
    call setLedOn    
    ld a, $55   ; fill all memory with $55 = 1010101
    ld (hl), a
    inc hl   
    ld c, $01      ; turn led on port zero off
    call setLedOff    
    djnz memFill
    
    ld hl, $0420    ; start of ram at $400 but need to allow 32bytes for stack
    ld b, $ff      ; 255 bytes to check (with more mem need nested loop)
memReadBack:    ;; now check that the value writtin can be read back
    push bc
    
    ld b, $55
    ld a, (hl)
    cp b
    jp nz, failedMemCheck
    
    pop bc
    djnz memReadBack
    ld c, $00     ;; if we got to end then set the port zero led on
    call setLedOn
    halt
    ;;; if we failed memory check then flash the led on port zero
failedMemCheck:
    ld c, $00     ;; if we got to end then set the port zero led on
    call setLedOn
    ld c, $00     ;; if we got to end then set the port zero led on
    call setLedOff
    jp failedMemCheck
    halt

;;; some subroutines        
setLedOn:  ; port stgored in c, a register preserver
    push af
    ld a, 1
    out (c), a
    pop af
    ret
setLedOff:  ; port stgored in c, a register preserver
    push af
    ld a, 0
    out (c), a
    pop af
    ret    
#END

