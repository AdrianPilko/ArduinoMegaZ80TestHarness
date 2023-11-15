#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines
extern const char ROM_image[] ="\
:1800000031F003AFD300D301D3023E01D3003E01D3013E01D30206302A\
:1800180010FEAFD300D301D30221000406FF3E01D3013E00D3013E55B5\
:18003000772310F221000406FF3E01D3013E00D3011E557EBBC24F0010\
:0C00480010EF3E01D300763E01D302769B\
:00000001FF\
\0";

#endif


