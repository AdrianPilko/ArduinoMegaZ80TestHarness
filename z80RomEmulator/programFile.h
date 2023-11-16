#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines

// program is a memcheck is setup for MAKE_ROM_AND_RAM_SMALL
extern const char ROM_image[] ="\
:180000003114022114021E557B777EBBC2140023C30800763E01D3017F\
:18001800220002060821FFFFE510FA0608E13EFFBCC24000BDC24000E7\
:1800300010F33E01D30021BEEF220202C33F00763E01D30121FBAD2239\
:030048000202763B\
:04020000FFFFFFFFFE\
:00000001FF\
\0";


#endif


