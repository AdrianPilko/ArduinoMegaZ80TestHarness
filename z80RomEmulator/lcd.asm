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

#define ROM_SIZE $8000
#define SIZE_OF_SYSTEM_VARIABLES $0004
#define STACK_SIZE_BYTES $0008
#define STACK_BOTTOM $0010+ROM_SIZE+SIZE_OF_SYSTEM_VARIABLES
#define RAM_SIZE $8000   ;;; this will be checked
    
    .org 0
    ld  sp , STACK_BOTTOM 
    
    
    ld hl,InitCommandList
    call waitLCD
loopLCDInitCommands
    ld a, (hl)
    cp $ff
    jp z, startOutChars
    out (lcdRegisterSelectCommand), a     ; send command to lcd (assuming lcd control port is at 0x00)
    inc hl
    jp loopLCDInitCommands

startOutChars:
    ld hl, BootMessage
loopLCDBootMessage:         
    call waitLCD 
    ld a, (hl)
    cp $ff
    jp z, afterDisplayText
    out (lcdRegisterSelectData), a
    inc hl
    jp loopLCDBootMessage
    
afterDisplayText:

    ld a,$c0        ; Set DDRAM address to start of the second line (0x40)
    out (lcdRegisterSelectCommand), a     ; Send command to LCD     
    ld hl, memcheckResultText
displayLCDMemCheckResultText:
    call waitLCD 
    ld a, (hl)
    cp $ff
    jp z, displayResult
    out (lcdRegisterSelectData), a
    inc hl
    jp displayLCDMemCheckResultText

displayResult:
    call waitLCD    
    
    ;write $55 to RAM then read back
    ld a, 5
    ld (STACK_BOTTOM), a   
    ld a, (STACK_BOTTOM)  
    ld b, $30   ; to convert to ascii, needs more to do 2 digits (isolat high nibble then low)
    add a, b
    out (lcdRegisterSelectData), a

    halt

waitLCD:    
waitForLCDLoop:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,waitForLCDLoop
    ret 
    
InitCommandList:
    .db $38,$0e,$01,$06,$ff
BootMessage:
    .db "Z80 byteForever",$ff
memcheckResultText:
    .db "Memcheck=",$ff
#END    
