
;;; memory model:
;;; 0x0000 to 7fff      - ROM
;;; 0x8000 to to 0xffff - RAM
;;; 
#define ROM_SIZE $7fff
#define SIZE_OF_SYSTEM_VARIABLES $0004
#define STACK_BOTTOM $ffff
#define RAM_START $8000  

#define lcdRegisterSelectCommand $00
#define lcdRegisterSelectData $01    

    .org $0
    
    ld  sp , STACK_BOTTOM 
    
    call initialiseLCD
    ld hl, RAM_START   ; this will overwrite srtack but is ok because nothing on it at the moment!
    ld e, $55       ; fill all memory with $55 = 1010101
memFillAndCheck:
    ld a, e   
    ld (hl), a
    ld a, (hl)
    cp e
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
mainMonitoLoop:
    ;read character from keypad - which is on port 1
    
    ; echo the character to the display
    
    ; if character is "S" - show memory
    ;call getAddress_ResHL    
    ; hl now contains the value read from keypad for start address,    
    ;call getAddress_ResHL
    jp mainMonitoLoop
    
    halt

getAddress_ResHL:
    ;; todo for now just return with hl = value read from keypad
    ld hl, $0000
    ret
    
postFail
    ld hl, $ADFB
    ld (POST_RESULT), hl    
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
    
setLCDRow2:
    call waitLCD
    ld a,$c0        ; Set DDRAM address to start of the second line (0x40)
    out (lcdRegisterSelectCommand), a     ; Send command to LCD         
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
    call hprint16    
    call waitLCD 
    ld a, 'h'    
    out (lcdRegisterSelectData), a
    ret
    

waitLCD:    
waitForLCDLoop:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,waitForLCDLoop    
    ret 
    
hprint16  ; print one 2byte number stored in location $to_print modified from hprint http://swensont.epizy.com/ZX81Assembly.pdf?i=1
	;ld hl,$ffff  ; debug check conversion to ascii
    ;ld ($to_print), hl
    
	ld hl,$to_print+$01	
	ld b,2	
hprint16_loop	
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
	djnz hprint16_loop
	ret	  

ConvertToASCII:
    ; assuming the value in register a (0-15) to be converted to ascii
    ; convert the value to its ascii representation
    add a, '0'       ; convert value to ascii character
    cp  '9'        ; compare with ascii '9'
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

