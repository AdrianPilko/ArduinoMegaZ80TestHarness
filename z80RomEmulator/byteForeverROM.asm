;;; assume we have memory model as such
;;; low  1024 bytes $0000 to $03ff are rom
;;; high 1024 bytes $0400 to $07ff are RAM
;;; does not set stack pointer, previous tests should not working properly yet
#define ROM_SIZE $8000
#define SIZE_OF_SYSTEM_VARIABLES $0004
#define STACK_SIZE_BYTES $0008
#define STACK_BOTTOM $0010+ROM_SIZE+SIZE_OF_SYSTEM_VARIABLES
#define RAM_SIZE $8000   ;;; this will be checked
    
    ;;; initialise LEDs on port zero and one to be off
    .org $0
    
    
#define lcdRegisterSelectCommand $00
#define lcdRegisterSelectData $01

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
    jp z, memFillAndCheck
    out (lcdRegisterSelectData), a
    inc hl
    jp loopLCDBootMessage    


    ld hl, STACK_BOTTOM   ; start of ram at $200 for small ram in emulator    
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
    ;; write the value of hl to memory location $201
    ld (RAM_MAX_VAR), hl

    ;; test the stack
    ld b, STACK_SIZE_BYTES    
stackPushCheckLoop:         
    ld hl, $ffff
    push hl
    djnz stackPushCheckLoop
    
    ld b, STACK_SIZE_BYTES
stackCheckLoop:    
    pop hl    
    ld a, $ff       
    cp h
    jp nz, postFail
    cp l
    jp nz, postFail    
    djnz stackCheckLoop
    
    ;;; also write post result to POST_RESULT $BEEF = pass $FBAD = fail
    ;; as in memory it's low byte high byte, reverse $EFBE = pass $ADFB = fail
    ld hl, $EFBE 
    ld (POST_RESULT), hl

   
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
    ld hl, (RAM_MAX_VAR)
    ld a, h       
    ld b, $30   ; to convert to ascii, so even though we're only adding 2+2 need another add
    add a, b
    out (lcdRegisterSelectData), a
    call waitLCD
    ld a, l
    ld b, $30   ; to convert to ascii, so even though we're only adding 2+2 need another add
    add a, b
    out (lcdRegisterSelectData), a      

runMonitor:   
    ;; machine code monitor :::: TODO!
    
    ;; initialise and clear display on port 0
    ;; output the prompt - use ">"
mainMonitoLoop:
    ;read character from keypad - which is on port 1
    
    ; echo the character to the display
    
    ; if character is "S" - show memory
    call getAddress_ResHL    
    ; hl now contains the value read from keypad for start address,
    
    
    call getAddress_ResHL
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

waitLCD:    
waitForLCDLoop:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,waitForLCDLoop
    ret 
    
    
    .org ROM_SIZE  
RAM_MAX_VAR:
    .dw $ffff
POST_RESULT:    
    .dw $ffff    
InitCommandList:
    .db $38,$0e,$01,$06,$ff
BootMessage:
    .db "Z80 byteForever",$ff
memcheckResultText:
    .db "Memcheck=",$ff    
#END

