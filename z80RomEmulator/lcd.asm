;
; code from bread80.com didn't work because it relied on slow arduino clock
; I've tried to correct this using delay loop to slow output down
; still not sure if hardware is correct either
; also there is only on lcd display port
;code made longer because my ram and rom select isn't working
; only have rom, so can't use stack todo call returns
;; lcd port, is only one output port for lcd
#define lcdPort $00
    
    .org 0
    
    ld a, $3f ; 8bit interface
    out (lcdPort), a
    ld b, $ff   ; delay toop
delay1:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay1

    ld a,$0f    ; display on, cursor on
    out (lcdPort), a

    ld b, $ff   ; delay toop
delay2:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay2

    ld a, $01    ; clear    
    out (lcdPort), a

    ld b, $ff   ; delay toop
delay3:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay3

    ld a, $06    ; left to right
    out (lcdPort), a

    ld b, $ff   ; delay toop
delay4:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay4

    ld hl, HELLO ; load address of first character in hello world
    
writeTextLoop:    
    ld a, (hl)
    cp $ff
    jp z, endProgram
    out (lcdPort), a
    inc hl

    ld b, $ff   ; delay toop
delay5:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay5

    jr writeTextLoop

endProgram:
    halt   

HELLO:    
    .db "Hello, world",$ff
#END    
