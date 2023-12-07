
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
    call setLCDRow2
    
    ;; initialise some variables
    xor a
    ld (commandFound), a    
    ld (paramCharCount), a
    ld hl, $0000
    ld (paramStrPtr), hl
    
    
mainMonitoLoop:
    call clearDisplay
    ;call demoUsingLCD
    
    ;read character from keypad - which is on port 1
    ; this involves scanning the rows, then reading the columns for a 1 set
    ; we have a 4 * 4 key pad
    ;  7 8 9 A     
    ;  4 5 6 B
    ;  1 2 3 C
    ;  R F E D      ; the hex numbers are as normal and R represents return/enter in the monitor
    ;
    ;

keyboardScanInit:
    ld c,keypadInOutPort  ;Port address
    ld a,1              ;Row to scan
    ld d,0              ;Row index    
keypadScanLoop:         
    ld c, keypadInOutPort    
    out (c), a    ; put row count out on the keypad port
    in e, (c)     ; immediately read back in    
    jp nz, keyFoundinRegARow_DCol
    inc d
    rlca
    jr nc,keypadScanLoop 

    
    jp keyboardScanInit 
    

;########################### START OF BREAD 80
;;;; understand how it works then rewrite



keyFoundinRegARow_DCol:              ;We found a pressed key
                        ;Print row index (decimal) and column (binary)
                        
    push bc
    ld b, $ff
waitLoopAfterKeyFound1:    
    push bc
    ld b, $3f
waitLoopAfterKeyFound2:       
 
    djnz waitLoopAfterKeyFound2
    pop bc
    djnz waitLoopAfterKeyFound1
    pop bc
    
    ;A=Row bit
    ;D=Row index
    ;E=Returned column
    xor a               ;A=0 (index counter)
column_index_loop:
    rr e                ;Rotate E right until we find the bit which is set
    jr c,column_index_done
    inc a               ;Otherwise, inc index and loop again
    jr column_index_loop
    
column_index_done:
    ;A=Column index
    ;D=Row index
    ;Calculate key number (index into look up table)
    ;We'll end up with a value, in A, 0000ccrr where cc is the column index
    ;and rr is the row index.
    ld e,a              ;Save column into E
    
    ld a,-4             ;Our keypad only uses the low 4 bits of the row
    add a,d             ;address so we need to subtract 4 from the index.
                        ;We do this by adding -4 to keep the code shorter.
                        ;We could simplify this with known specific hardware.
                        
    add a,a             ;Shift column left 2 bits. Adding a number to itself
    add a,a             ;is the same as a left shift but ADD A,A is faster on Z80.
    
    add a,e             ;Add on row. A = final key value.
    
;Lookup the key value in the table
    ld e,a              ;Pop into E for outputting
    
    ;Lookup character in lookup table
    ld hl,keypadLookup    ;Table address
    ld c,a              ;Char offset in BC
    ld b,0
    add hl,bc
    ld l,(hl)           ;Key in L, saved for later

   
    call waitLCD
    
    ld c,lcdRegisterSelectData       ;LCD command port
    out (c),l           ;*****Output the character
    ld a, l
    ld (charFoundASCII), a
    
    ; the processing will be 1 hex digits for command, what follows depends on the command
    ; checking of command not yet implemented
    ; 0 - read memory   - 0 <from start sddress16bits><to end address16bits> 

    ld a, (commandFound)
    cp 1
    jp z, waitingParam

    ld a, l   ; if the commandFound is zero then we need another  
    cp '0'
    jp nz, waitingParam
  
    call displayEnterAddress
    ld a, 1
    ld (commandFound), a
    xor a
    ld (paramCharCount), a
    ld hl, paramStrMem
    ld (paramStrPtr), hl
    jp keyboardScanInit
waitingParam:    
    ld a, (paramCharCount)
    inc a
    ld (paramCharCount), a
    cp 9 
    jp z, paramComplete    
    ld a, (charFoundASCII)
    ld hl, (paramStrPtr)
    ld (hl), a
    inc hl
    ld (paramStrPtr), hl
   
    jp keyboardScanInit
paramComplete:
    ;;; process command 
    ;; (for now just set command found back to zero)
    ;; todo print memory represented by the start end in the 8 chars
    
    xor a
    ld (commandFound), a    
    ld (paramCharCount), a
    ;;call displayAddressFromParamStrMem    
    call displayEnterCommand
        
    jp keyboardScanInit    ;Loop infinitely
    

;############################# END OF BREAD 80
    
    
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
    ret
    
displayResult:
    call waitLCD    
    ld hl, (RAM_MAX_VAR)
    ld (to_print), hl
    call hexprint16    
    call waitLCD 
    ld a, 'h'    
    out (lcdRegisterSelectData), a
    ret
    
displayEnterAddress:        
    call setLCDRow1
    ld hl, EnterAddressPromptText    
displayEnterAddressLoop:
    call waitLCD 
    ld a, (hl)
    cp $ff
    jp z, displayEnterAddressRet
    out (lcdRegisterSelectData), a
    inc hl
    jp displayEnterAddressLoop
displayEnterAddressRet:    
    call setLCDRow2
    ret

displayAddressFromParamStrMem:
    ld hl, (paramStrMem)
    ;call convertASCIIBackToHexHL
    ;; now look up memory and display
    ret
    
convertASCIIBackToHexHL:    ; char array "pointer" stored in hl
;; got this next bit from chart gpt not working yet!!!
    ; ld hl, CharArray ; Assuming CharArray holds the ASCII characters
    ; ld de, HexArray  ; Destination array to hold hexadecimal representation
    ; ld bc, 4         ; Length of the character array (4 characters)
    

; ConversionLoop:
    ; ld a, (hl)      ; Load the ASCII character into register A
    ; call AsciiToHex ; Call subroutine to convert ASCII to hexadecimal
    ; ld (de), a      ; Store the hexadecimal value in HexArray
    ; inc hl          ; Move to the next character in the CharArray
    ; inc de          ; Move to the next position in the HexArray
    ; djnz ConversionLoop ; Continue the loop until all characters are converted

    ; ; Your code continues here...

; AsciiToHex:
    ; cp '0'          ; Compare with '0' (ASCII value)
    ; jr c, NotNum    ; If less than '0', it's not a number

    ; cp '9' + 1      ; Compare with ('9' + 1)
    ; jr nc, NotNum   ; If greater than '9', it's not a number

    ; sub '0'         ; Convert digit from ASCII to decimal
    ; ret             ; Return with the hexadecimal value in A

; NotNum:
    ; cp 'A'          ; Compare with 'A' (ASCII value for A)
    ; jr c, NotAlpha  ; If less than 'A', it's not an uppercase letter

    ; cp 'F' + 1      ; Compare with ('F' + 1)
    ; jr nc, NotAlpha ; If greater than 'F', it's not an uppercase letter

    ; sub 'A' - 10    ; Convert uppercase letter from ASCII to decimal (subtract 10)
    ; ret             ; Return with the hexadecimal value in A

; NotAlpha:
    ; cp 'a'          ; Compare with 'a' (ASCII value for a)
    ; jr c, Invalid   ; If less than 'a', it's neither uppercase nor lowercase letter

    ; cp 'f' + 1      ; Compare with ('f' + 1)
    ; jr nc, Invalid  ; If greater than 'f', it's neither uppercase nor lowercase letter

    ; sub 'a' - 10    ; Convert lowercase letter from ASCII to decimal (subtract 10)
    ; ret             ; Return with the hexadecimal value in A

; Invalid:
    ; ; Handle invalid characters (optional)
    ; ret             ; Return

; CharArray:
    ; .db '1', 'A', 'b', 'F' ; Example ASCII character array
; HexArray:
    ; .ds 4               ; Space to store hexadecimal representation
    ret

    
displayEnterCommand:
    call setLCDRow1
    ld hl, displayEnterCommandText    
displayEnterCommandLoop:
    call waitLCD 
    ld a, (hl)
    cp $ff
    jp z, displayEnterCommandRet
    out (lcdRegisterSelectData), a
    inc hl
    jp displayEnterCommandLoop
displayEnterCommandRet:    
    call setLCDRow2
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
    push af
    call waitLCD
	ld a, $01
	ld (lcdRegisterSelectCommand), a
    pop af
	ret 

setLCDRow1:
    call waitLCD
    ld a, $80         ; Set DDRAM address to start of the first row
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
    ret 

setLCDRow2:
    push af
    call waitLCD
    ld a, $80+ $40        ; Set DDRAM address to start of the second line (0x40)
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
    pop af
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
EnterAddressPromptText
    .db "start/end addr: ",$ff
displayEnterCommandText:    
    .db "enter command:  ",$ff
keypadLookup:
    .db "DE0FC987B654A321"    
;;; ram variables    
    .org RAM_START
RAM_MAX_VAR:
    .dw $ffff
POST_RESULT:    
    .dw $ffff    
to_print:
    .dw $0000
commandFound:
    .db $00
paramStrMem:
    .db "00000000",$ff,$ff,$ff
paramStrPtr:
    .dw $0000
paramCharCount:
    .db $00
charFoundASCII
    .db $00
#END

