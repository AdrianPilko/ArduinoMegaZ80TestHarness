#ifndef PROGRAMFILE
#define PROGRAMFILE
// simple toggle port zero to flash led (first simple loop one uses nested loop and stack flashes 255*15 times)
extern const char ROM_image[] ="31FF0006FF3E01D30000003E00D300C5060F3E00D30100003E01D30110F4C110E4\0";

//extern const char ROM_image[] ="31FF0006FFC5060F3E01D300AFD3013E01D30110F7AFD300C110EA763E01D301C9AFD301C9\0";
//extern const char ROM_image[] ="31FF000603C50603AFD3003E01D300AFD3013E01D30110F0C110EA76\0";
//extern const char ROM_image[] ="31FF003EFFF53E00D300F1D3003E00D3013E01D301C3030076\0"; // with stack
//extern const char ROM_image[] ="3E00D3003E01D3003E00D3013E01D301C3000076\0";


// extern const char ROM_image[] = "062F033effd301afD30110f73effD30076\0";    // simple code to output to 2 leds
//extern const char ROM_image[] = "0E003EFFED790E003E00ED79C3000076\0";  /// flash led on port zer0 (pin 10 of arduino mega)

//extern const char ROM_image[] = "0E003EFFED790E003E00ED790E013EFFED790E013E00ED79C3000076\0"; // flash both LED on off

// this requires 2048 size ram, built from memcheck.asm 
//extern const char ROM_image[] ="AFD300D30121000406023E01D3013E00D3013E55772310F2\
//3E01D30021000406023E01D3013E00D3011E557EBBCA3B00\
//10EF3E01D3003E01D30176C33B0076\0";

#endif


