;; port definitions
#define lcdRegisterSelectCommand $00   ; all zero address including line 0 which is connected to the LCD  ReadSelect (RS) pin 
#define lcdRegisterSelectData $01      ; all zero address except line 0 which is connected to the LCD  ReadSelect (RS) pin 
#define keypadInOutPort $20             ; A6 high the rest low
    
    .org 0   
    
    ld sp, $ffff
    call initialiseLCD    
keyboard_scan:          ;Loop for each row
    call setLCDRow1
    ld a, 00000001b
    call getAndPrintKeypadA    
    ld a, 00000010b
    call getAndPrintKeypadA    
    ld a, 00000100b
    call getAndPrintKeypadA
    ld a, 00001000b
    call getAndPrintKeypadA
    
    call setLCDRow2
    ld a, 00010000b
    call getAndPrintKeypadA    
    ld a, 00100000b
    call getAndPrintKeypadA 
    ld a, 01000000b
    call getAndPrintKeypadA 
    ld a, 10000000b
    call getAndPrintKeypadA   
    jr keyboard_scan    ;Loop infinitely (because this is a test)

getAndPrintKeypadA:
    out ($20),a           ;Output row
    in a, ($20)           ;Read columns    
    call hexprint8
    call waitLCD
    ld a, ' '
    out (lcdRegisterSelectData), a    
    ret    

hexprint8 		
	push af ;store the original value of a for later
    call waitLCD 
    pop af
    push af ;store the original value of a for later
	and $f0 ; isolate the first digit    
	rrca
	rrca
	rrca
	rrca  
    call ConvertToASCII
	out (lcdRegisterSelectData), a
    call waitLCD 
	pop af ; retrieve original value of a
	and $0f ; isolate the second digit
    call ConvertToASCII       
	out (lcdRegisterSelectData), a
	ret

ConvertToASCII:
    ; assuming the value in register a (0-15) to be converted to ascii
    ; convert the value to its ascii representation
    add a, '0'       ; convert value to ascii character
    cp  ':'        ; compare with ascii '9'
    jr  nc, ConvertToASCIIdoAdd     ; jump if the value is not between 0-9
    jp ConvertToASCII_ret
ConvertToASCIIdoAdd:    
    add a, 7     ; if greater than '9', adjust to ascii a-f
ConvertToASCII_ret:
    ret              ; return from subroutine
    
    
waitLCD:    
    push af
waitForLCDLoop:         
    
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,waitForLCDLoop    
    pop af
    ret 

;;; "generic" display code
; self evident, this clears the display
clearDisplay:
    call waitLCD
	ld a, $01
	ld (lcdRegisterSelectCommand), a
	ret 

setLCDRow1:
    call waitLCD
    ld a, $80         ; Set DDRAM address to start of the first row
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
    ret     
    
setLCDRow2:
    call waitLCD
    ld a, $80+ $40        ; Set DDRAM address to start of the second line (0x40)
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
    ret   

    
initialiseLCD:
    ld hl,InitCommandList
    call waitLCD
loopLCDInitCommands
    ld a, (hl)
    cp $ff
    jp z, initialiseLCDRET
    out (lcdRegisterSelectCommand), a     ; send command to lcd (assuming lcd control port is at 0x00)
    inc hl
    jp loopLCDInitCommands
initialiseLCDRET:    
    ret
;;; rom "constants"
InitCommandList:
    .db $38,$0e,$01,$06,$ff
#END