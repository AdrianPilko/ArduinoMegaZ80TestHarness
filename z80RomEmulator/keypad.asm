; Define I/O port addresses
KEYPAD_OUT equ 20h ; Keypad output port
KEYPAD_IN  equ 21h ; Keypad input port

;; to read a keypad setup as a matrix, we have to first set the 
;; output buffer then read the input latch which tells us which 
;; keys were press of course we have to scan the whole matrix

START:
    ld a, 0FFh ; Initialize mask for scanning rows
    out (KEYPAD_OUT), a ; Send mask to keypad output port
    call DELAY_MS ; Insert delay (if needed) for stability
    
LOOP:
    in a, (KEYPAD_IN) ; Read the keypad input
    and 0Fh ; Mask the lower 4 bits
    
    cp 0Fh ; Check if all keys are released
    jp z, LOOP ; If all keys are released, continue scanning
    
    ; Check which key is pressed
    cp 0Eh ; Check if first row is pressed
    jr z, FIRST_ROW_PRESSED
    
    cp 0Dh ; Check if second row is pressed
    jr z, SECOND_ROW_PRESSED
    
    cp 0Bh ; Check if third row is pressed
    jr z, THIRD_ROW_PRESSED
    
    cp 07h ; Check if fourth row is pressed
    jr z, FOURTH_ROW_PRESSED
    
    jp LOOP ; If no key is pressed, continue scanning
    
FIRST_ROW_PRESSED:
    ; todo
    jp LOOP ; Continue scanning
    
SECOND_ROW_PRESSED:
    ; todo
    jp LOOP ; Continue scanning
    
THIRD_ROW_PRESSED:
    ; todo
    jp LOOP ; Continue scanning
    
FOURTH_ROW_PRESSED:
    ; todo
    jp LOOP ; Continue scanning
    

; assuming a 1MHz clock we have a 1 / (1000000 / ((4 + 8) * 255)+13)) seconds, 3073/1000000 = 0.003073seconds (3msec 
delayLoop_3msec:
    nop              ; nop is 4 clock cycles
    djnz delayLoop   ;13 when b reaches zero, 8 clock cycles otherwise
    ret


