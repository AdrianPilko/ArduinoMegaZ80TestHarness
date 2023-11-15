#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines
extern const char ROM_image[] ="\
:1800100021F003F906053E01D301000000003E003E003E00D301D3014B\
:09002800D3010000000010E6768F\
:030040007676765B\
:00000001FF\
\0";

#endif


