#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines

// program is a memcheck (only does 255 bytes currentlty)
extern const char ROM_image[] ="\
:1800000031F003AFD300D301D3023E01D3003E01D3013E01D30206302A\
:180018000010FDAFD300D301D30221000406FF3E55772310FA21000412\
:1500300006FF1E557EBBC2400010F73E01D300763E01D30276EF\
:00000001FF\
\0";

#endif


