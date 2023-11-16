#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines

// program is a memcheck is setup for MAKE_ROM_AND_RAM_SMALL
extern const char ROM_image[] ="\
:180000003110002110021E557B777EBBC2140023C30800763E01D30288\
:040018002201027649\
:02020100FFFFFD\
:00000001FF\
\0";


#endif


