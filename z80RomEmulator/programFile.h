#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines
extern const char ROM_image[] ="\
:03000000C310002A\
:1800100021F003F90605000000000000000000000000000000000000C0\
:0C0028000000000000000000C31600767D\
:030040007676765B\
:00000001FF\
\0";

#endif


