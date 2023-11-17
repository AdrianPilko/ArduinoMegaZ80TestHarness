; found this at bread80.com/2020/09/04/couch-to-64k-part3-adding-a-character-lcd-display-to-our-z880-breadboard-computer    
    
;; LCD common IO port    
#define lcd_command $00  
;; LCD data IO port
#define lcd_data $01  
    
    .org 0
    
    ld a, $3f ; function set: 8bit interface
    out (lcd_command), a
    ld a,$0f    ; display on, cursor on
    out (lcd_command), a
    ld a, $01    ; clear display    
    out (lcd_command), a
    ld a, $06    ; entry mode left to right, no shift
    out (lcd_command), a
    ld hl, message
    
message_loop:    
    ld a, (hl)
    and a
    jr z, done
    out (lcd_data), a
    inc hl
    jr message_loop ; loop back for next character
done:
    halt
message:    
    .db ">HELLO< >DOCTOR< >NAME< >CONTINUE< >YESTERDAY< >TOMMOROW<",0
#END    
