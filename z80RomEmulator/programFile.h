#ifndef PROGRAMFILE
#define PROGRAMFILE

// the rom image can be copied from the .obj file which is an z80 srec format, as output by tasm
// the arduino code parses it doirectly provided it has the C '\0' string null terminator added and  '\' if on ,multiple lines

// program is a memcheck is setup for MAKE_ROM_AND_RAM_SMALL
//extern const char ROM_image[] ="\
//:180000003110022110041E557B777EBB23C21400C30800763E01D30284\
//:040018002201027649\
//:020201000000FB\
//:00000001FF\
//\0";
extern const char ROM_image[] ="\
:0D00000000003E100610802101027776C935\
:00000001FF\
\0";


#endif


