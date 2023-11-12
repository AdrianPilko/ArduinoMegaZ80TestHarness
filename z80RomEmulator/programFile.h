#ifndef PROGRAMFILE
#define PROGRAMFILE
// extern const char ROM_image[] = "062F033effd301afD30110f73effD30076\0";    // simple code to output to 2 leds
//extern const char ROM_image[] = "0E003EFFED790E003E00ED79C3000076\0";  /// flash led on port zer0 (pin 10 of arduino mega)

//extern const char ROM_image[] = "0E003EFFED790E003E00ED790E013EFFED790E013E00ED79C3000076\0"; // flash both LED on off

// this requires 2048 size ram, built from memcheck.asm 
extern const char ROM_image[] ="AFD300D30121000406023E01D3013E00D3013E55772310F2\
3E01D30021000406023E01D3013E00D3011E557EBBCA3B00\
10EF3E01D3003E01D30176C33B0076\0";








// program to check memory, does not use stack

// extern const char ROM_image[] = "217F0006FF3E55772310FA76\0";  memfile from 7F to 17E

#endif


