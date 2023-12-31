;; port definitions
#define lcdRegisterSelectCommand $00   ; all zero address including line 0 which is connected to the LCD  ReadSelect (RS) pin 
#define lcdRegisterSelectData $01      ; all zero address except line 0 which is connected to the LCD  ReadSelect (RS) pin 
#define keypadInOutPort $20            ; A6 high the rest low
    
    .org 0   
    
    ld sp, $ffff
    call initialiseLCD
keyPadScan:
    ld a, 1
    ld b, 7
    ld d, 0
keyboardRowScan:          ;Loop for each row    
    ld c, keypadInOutPort
    out (c),a   ; put a out on databus on the keypad port        
    in e, (c)   ; read keypad port into e
    cp e        
    call nz,keyFoundLocal     ;If non-zero then a key is being pressed    
       
    ld b, $ff
keyboardRowScandelayL1:    ; add a delay so we can see what's going on
    push bc
    ld b, $7f
keyboardRowScandelayL2:    ; add a delay so we can see what's going on
    djnz keyboardRowScandelayL2
    pop bc    
    djnz keyboardRowScandelayL1    
    
    rlca
    
    jp keyboardRowScan

keyFoundLocal:
    push af
    call clearDisplay    
    call setLCDRow1
    ld hl, lcdTextSettingOut    
    call printStringToLCD_HL    
        
    call hexprint8    
    push af  ; preserve af
    call setLCDRow2
    ld hl, gotCodeText
    call printStringToLCD_HL      
    
    ld a, e    
    call hexprint8    
    pop af

    ret


ClearAndPrintA
    call clearDisplay    
    call setLCDRow1
    call hexprint8 
    ret

; keyFound:
    ; push af
    ; ;; row is in d, column is in e
    ; ;; if we have 4x4
    ; ;; 1 2 3 A
    ; ;; 4 5 6 B
    ; ;; 7 8 9 C
    ; ;; # 0 . D 
    ; ;; store in memory at keypadChars 123A456B789C#0.D         
    ; ld hl, keypadChars
    ; ld a, e
; keypadFindCharLoopCol:
    ; inc hl   ; inc hl the number of times it takes e (a)
             ; ; to times fall off end
    ; rra
    ; jp nz, keypadFindCharLoopCol
    ; ld b, d
; keypadFindCharLoopRow:
    ; inc hl
    ; djnz keypadFindCharLoopRow
    ; ld a, (hl)
    ; call hexprint8
    ; pop af
    ; jp afterKeyFoundCall

getAndPrintKeypadA:
    out ($20),a           ;Output row
    in a, ($20)           ;Read columns    
    call hexprint8
    call waitLCD
    ld a, ' '
    out (lcdRegisterSelectData), a    
    ret    


printStringToLCD_HL:    
    push af
printStringToLCD_HLLoop:
    call waitLCD 
    ld a, (hl)
    cp $ff
    jp z, printStringToLCD_HLLoopEND
    out (lcdRegisterSelectData), a
    inc hl
    jp printStringToLCD_HLLoop
printStringToLCD_HLLoopEND:    
    pop af
    ret 
    
hexprint8:
    push af ; preserve af		
    push af ; preserve af		
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
    pop af  ; restore af
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
    push af
    call waitLCD
	ld a, $01
	ld (lcdRegisterSelectCommand), a
    pop af 
	ret 

setLCDRow1:
    push af
    call waitLCD
    ld a, $80         ; Set DDRAM address to start of the first row
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
    pop af
    ret     
    
setLCDRow2:
    push af
    call waitLCD
    ld a, $80+ $40        ; Set DDRAM address to start of the second line (0x40)
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
    pop af
    ret   

    
initialiseLCD:
    ld hl,InitCommandList
    call waitLCD
loopLCDInitCommands:
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
keypadChars:
    .db "123A456B789CF0E"
lcdTextSettingOut:
    .db "Setting bits ",$ff
gotCodeText:
    .db "got bits ",$ff
#END