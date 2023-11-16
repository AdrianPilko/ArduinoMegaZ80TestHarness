#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines

// program is a memcheck is setup for MAKE_ROM_AND_RAM_SMALL
extern const char ROM_image[] ="\
:180000003114022114021E557B777EBBC2140023C30800762200020668\
:180018000821FFFFE510FA0608E13EFFBCC24500BDC2450010F321BE25\
:18003000EF220202C33700CD4100CD4100C3370076210000C921FBAD6A\
:040048002202027618\
:04020000FFFFFFFFFE\
:00000001FF\
\0";


#endif


