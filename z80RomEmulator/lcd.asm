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
 
delayLoop1:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop1 
    
    ld a, $38        ; function set: 8-bit mode, 2 lines, 5x8 font
    out (lcdRegisterSelectCommand), a     ; send command to lcd (assuming lcd control port is at 0x00)

delayLoop2:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop2
    
    ld a, $0e        ; display on/off control: display on, cursor on, blink on
    out (lcdRegisterSelectCommand), a     ; send command to lcd

delayLoop3:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop3
    
    ld a, $01        ; clear display command
    out (lcdRegisterSelectCommand), a     ; send command to lcd

delayLoop4:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop4
    
    ld a, $06        ; entry mode set: increment cursor position, no display shift
    out (lcdRegisterSelectCommand), a     ; send command to lcd

delayLoop5:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop5
    
    ld a, 'H'
    out (lcdRegisterSelectData), a

delayLoop6:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop6    
    
    ld a, 'E'
    out (lcdRegisterSelectData), a
    
delayLoop7:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop7
    
    ld a, 'L'
    out (lcdRegisterSelectData), a

delayLoop8:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop8
        
    ld a, 'L'    
    out (lcdRegisterSelectData), a    

delayLoop9:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop9
        
    ld a, 'O'    
    out (lcdRegisterSelectData), a    

delayLoop10:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop10
        
    ld a, ','    
    out (lcdRegisterSelectData), a    

delayLoop11:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop11
        
    ld a, 'W'    
    out (lcdRegisterSelectData), a    

delayLoop12:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop12
        
    ld a, 'O'    
    out (lcdRegisterSelectData), a    

delayLoop13:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop13
        
    ld a, 'R'    
    out (lcdRegisterSelectData), a    

delayLoop14:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop14
        
    ld a, 'L'    
    out (lcdRegisterSelectData), a    

delayLoop15:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop15
        
    ld a, 'D'    
    out (lcdRegisterSelectData), a    
        
delayLoop16:         
    in a,(lcdRegisterSelectCommand)  
    rlca              
    jr c,delayLoop16
        
    ld a, '!'    
    out (lcdRegisterSelectData), a    
            
    
    halt

#END    
