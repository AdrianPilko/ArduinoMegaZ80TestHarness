#ifndef PROGRAMFILE
#define PROGRAMFILE
   // extern const char ROM_image[] = "062F033effd301afD30110f73effD30076\0";    // simple code to output to 2 leds

/*   program to fill some memory and flash led connected to port 1, then light led oon port zero at end
; initialise the CPU registers and check RAM
    ld a, 0       ; set led on out port 0 to off
    out (0x00), a
    ld a, 1       ; set led on out port 1 to off
    out (0x01), a
    
    ld hl, 0x3f    ; start of ram at 255 but need to allow 16bytes for stack
    ld b, 0x03      ; 255 bytes to check (with more mem need nested loop)

memFill:
    ld a, 0x43   ; fill all memory with 0x43
    ld (hl), a
    inc hl
    
    ld a, 1
    out (0x01), a    
    
    djnz memFill
    
    ld a, 1
    out (0x00), a
    ld a, 1
    out (0x01), a    
    halt



     
*/
extern const char ROM_image[] = "3E00D3003E01D301213F0006033E43776B10001000233E01D30110F63E01D3003E01D30176\0";

// program to fill memory from 1f to 0x1f + 0x3f with 0x43
//   extern const char ROM_image[] = "AF211F00063F3E43772310FC76\0";    
   
//    ld hl, 0x1f    
//    ld b, 0x3f     
//    ld a, 0x43   
//memFill:
//    ld (hl), a
//    inc hl
//    djnz memFill    
//    halt


#endif


