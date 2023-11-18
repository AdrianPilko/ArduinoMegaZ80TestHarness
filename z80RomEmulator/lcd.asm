;
; code from bread80.com didn't work, it relied on slow arduino clock
; also the port numbers look wrong.

; the wiring is as such on my version of the computer (https://github.com/AdrianPilko/Z80-computer)
;;the lcd E pin (enable) is NAND'd with the pin 15 of the U4 74ls138,
;;    --> the U4 (74ls138) pin A0 pin 1 is connected to Z80 A5
;;    --> the U4 (74ls138) pin A1 pin 2 is connected to Z80 A6
;;    --> U4 (74ls138) pin A2 pin 3 is connected to Z80 A7
;; The Z80 A0 is connected to the RS(register select) pin on the lcd display
;; The Z80 /RD (active low RD pin) NAND'd and connected to the R/W pin

;; datasheet for the lcd display: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
;; this all means:
;;     to write to the display as a commands RS has to be set to 0
;;     to write to the display as a data RS has to be set to 1
;;     we don't need to worry about the R/W pin (enable as that's handled by NAND'd /RD)
;;     we have to set the output port address to A5 high A6 low and A7 Low, the others apart from A0 fdon't matter

#define lcdRegisterSelectCommand $00
#define lcdRegisterSelectData $01
#define lcdEnableAddress $10    ; address pin 5 high on all other address set to zero, z80 only uses 255 ports with in/out instruction (8bit)
#define enableLCDANDRSCommand $10
#define enableLCDANDRSData $11
    
    .org 0
    
    ld a, $3f ; 8bit interface
    out (enableLCDANDRSCommand), a
    ld b, $ff   ; delay toop
delay1:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay1

    ld a,$0f    ; display on, cursor on
    out (enableLCDANDRSCommand), a

    ld b, $ff   ; delay toop
delay2:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay2

    ld a, $01    ; clear    
    out (enableLCDANDRSCommand), a

    ld b, $ff   ; delay toop
delay3:
    ld a, $55   ; waste some clock cycles
    add a, b
    djnz delay3

    ld a, $06    ; left to right
    out (enableLCDANDRSCommand), a

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
    out (enableLCDANDRSData), a
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
