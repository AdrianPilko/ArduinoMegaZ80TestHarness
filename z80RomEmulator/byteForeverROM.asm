
;;; memory model:
;;; 0x0000 to 7fff      - ROM
;;; 0x8000 to to 0xffff - RAM
;;; 
#define ROM_SIZE $7fff
#define SIZE_OF_SYSTEM_VARIABLES $0004
#define STACK_BOTTOM $ffff
#define RAM_START $8000  

;; port definitions
#define lcdRegisterSelectCommand $00   ; all zero address including line 0 which is connected to the LCD  ReadSelect (RS) pin 
#define lcdRegisterSelectData $01      ; all zero address except line 0 which is connected to the LCD  ReadSelect (RS) pin 
#define keypadInOutPort $20             ; A6 high the rest low

    .org $0
    
    ld  sp , STACK_BOTTOM 
    
    call initialiseLCD
    ld hl, RAM_START   ; this will overwrite srtack but is ok because nothing on it at the moment!
    ld e, $0       
memFillAndCheck:
    ld a, e   
    ld (hl), a
    ld a, (hl)
    cp e
    inc e
    jp nz, endMemCheck
    inc hl  
    jp memFillAndCheck
    ; only gets here if it didn't find top of ram?!
    halt
     
endMemCheck:
    dec hl   ; dec by one as check tested location+1
    ld  sp , STACK_BOTTOM 
    ;; write the value of hl to memory location $201
    ld (RAM_MAX_VAR), hl
    call setLCDRow2
    call printMemcheckResult     
    
runMonitor:   
    ;; machine code monitor :::: TODO!
    
    ;; initialise and clear display on port 0
    ;; output the prompt - use ">"
    ld hl, $0000
mainMonitoLoop:
    ;call clearDisplay
    ;call demoUsingLCD
    
    ; print contents of memory continuously
    ld b, $ff
waitLoopMain:       ;; waste some cycles to slow lcd output down a bit
    xor a
    add a, b
    djnz waitLoopMain
    
    call setLCDRow1
    ld b, $f
lcdClearLine1:    
    ld a, $20   ;space
    call displayCharacter   ; a stores character to display
    djnz lcdClearLine1
    
    call waitLCD
    call setLCDRow1    
    ld (to_print), hl
    push hl
    call hexprint16
    pop hl
    ld a, ':'
    call displayCharacter   ; a stores character to display
    ld a, (hl)
    call hexprint8    

    call setLCDRow2
    ld b, $f
lcdClearLine2:    
    ld a, $20   ;space
    call displayCharacter   ; a stores character to display
    djnz lcdClearLine2
    call waitLCD
    
    call setLCDRow2
    ld (to_print), hl
    push hl
    call hexprint16
    pop hl
    ld a, ':'
    call displayCharacter   ; a stores character to display    
    call waitLCD
    ld a, (hl)
    call hexprint8    
    inc hl     
    jr mainMonitoLoop
    ;read character from keypad - which is on port 1
    ; this involves scanning the rows, then reading the columns for a 1 set
    ; we have a 4 * 4 key pad
    ;  7 8 9 A     
    ;  4 5 6 B
    ;  1 2 3 C
    ;  R F E D      ; the hex numbers are as normal and R represents return/enter in the monitor
    ;
    ;
    ld b, 4
keypadScanLoop:    
    ld a, b
    out (keypadInOutPort), a    ; put row count out on the keypad port
    in a, (keypadInOutPort)     ; immediately read back in
    cp 1
    jp z, keyFoundinRegA
    djnz keypadScanLoop
    jp noKeyPressed
keyFoundinRegA:      
    ; echo the character to the display
    call hexprint8
    
    ; todo
    
    ; if first number entered, 
    ;      0 = display ram next 4 digits are address when R pressed next will execute
    ;      1 = enter address 4 digits are address R pressed next will execute
    ;      2 = enter data at current address, 4 digits are address R pressed next will execute
    
noKeyPressed:    
    jp mainMonitoLoop
    
    halt
  
initialiseLCD:
    ld hl,InitCommandList
    call waitLCD
loopLCDInitCommands
    ld a, (hl)
    cp $ff
    jp z, outputBootMessage
    out (lcdRegisterSelectCommand), a     ; send command to lcd (assuming lcd control port is at 0x00)
    inc hl
    jp loopLCDInitCommands
    
outputBootMessage:
    ld hl, BootMessage
loopLCDBootMessage:         
    call waitLCD 
    ld a, (hl)
    cp $ff
    jp z, initialiseLCD_ret
    out (lcdRegisterSelectData), a
    inc hl
    jp loopLCDBootMessage    
initialiseLCD_ret    
    ret
  

printMemcheckResult:    
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
    ld hl, (RAM_MAX_VAR)
    ld (to_print), hl
    call hexprint16    
    call waitLCD 
    ld a, 'h'    
    out (lcdRegisterSelectData), a
    ret
    
demoUsingLCD:
    ld b, $ff
    ld a, '#'
demoLCDLoop1:
    push bc
    call waitLCD
    out (lcdRegisterSelectData), a
    pop bc
    djnz demoLCDLoop1
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

moveCursorToPostion:  ;; b store the cursor position 
	call waitLCD
	ld a, $fe
    ld (lcdRegisterSelectCommand), a
	call waitLCD
	ld a, $80      ; start offset into ddram for cursor, if b=0 thats top left
	add a, b
    ld (lcdRegisterSelectCommand), a	
	ret

;;; make sure the lcd isn't busy - by checking the busy flag
waitLCD:    
    push af
waitForLCDLoop:         
    
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,waitForLCDLoop    
    pop af
    ret 
    
displayCharacter:    ; register a stores tghe character
    call waitLCD
    out (lcdRegisterSelectData), a
    ret 
    
hexprint16  ; print one 2byte number stored in location $to_print modified from hprint http://swensont.epizy.com/ZX81Assembly.pdf?i=1
	;ld hl,$ffff  ; debug check conversion to ascii
    ;ld ($to_print), hl
    
	ld hl,$to_print+$01	
	ld b,2	
hexprint16_loop	
    call waitLCD    
	ld a, (hl)
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
	dec hl
	djnz hexprint16_loop
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
    

;;; rom "constants"
InitCommandList:
    .db $38,$0e,$01,$06,$ff
BootMessage:
    .db "Z80 byteForever",$ff
memcheckResultText:
    .db "Memcheck=",$ff    
    
;;; ram variables    
    .org RAM_START
RAM_MAX_VAR:
    .dw $ffff
POST_RESULT:    
    .dw $ffff    
to_print:
    .dw $0000
#END

