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
;; datasheet for 74ls138 https://www.ti.com/lit/ds/symlink/sn74ls138.pdf
;; this all means:
;;     to write to the display as a commands RS has to be set to 0 ie z80 address pin 0 set to 0
;;     to write to the display as a data RS has to be set to 1  ie z80 address pin 0 set to 1
;;     we don't need to worry about the R/W pin (enable as that's handled by NAND'd /RD)
;;     we have to set z80 A5 low A6 low and A7 Low, the others apart from A0 don't matter

#define lcdRegisterSelectCommand $00
#define lcdRegisterSelectData $01
    
    .org 0
 
    ld hl,InitCommandList
delayLoop1:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop1 
    ld a, (hl)
    cp $ff
    jp z, startOutChars
    out (lcdRegisterSelectCommand), a     ; send command to lcd (assuming lcd control port is at 0x00)
    inc hl
    jp delayLoop1

startOutChars:
    ld hl, BootMessage
delayLoop2:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop2
    
    ld a, (hl)
    cp $ff
    jp z, afterDisplayText
    out (lcdRegisterSelectData), a
    inc hl
    jp delayLoop2
afterDisplayText:
    halt
InitCommandList:
    .db $38,$0e,$01,$06,$ff
BootMessage:
    .db 'Z80..byteForever',$ff
#END    
