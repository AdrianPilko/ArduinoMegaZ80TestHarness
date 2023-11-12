note that the assembler ouput has a format that needs tweaking
before loading into the char array in the arduino programFile.h

(NOTE stripasm.bat does some of the work required below)

the raw output in the .obj file is as such

:180000000E00CD4D000E01CD4D0031000421200406FF0E01CD46003EB8
:180018005577230E01CD4D0010F021200406FFC506557EB8C23800C15D
:1800300010F50E00CD4600760E00CD46000E00CD4D00C3380076F53E2F
:0C00480001ED79F1C9F53E00ED79F1C938
:00000001FF

1) the first 9 characters of each line have to be remove so in this case
the first row has :18000000 
2) the last 2 character also need removing from each line starting with "B8"
  in the example code above
3) the last line needs removing completely
4) add the following:   extern const char ROM_image[] = " at start and \0"; end
5) add \ at end of each line no spaces, except last line
6) copy paste into the programFile.h :

extern const char ROM_image[] = "0E00CD4D000E01CD4D0031000421200406FF0E01CD46003E\
5577230E01CD4D0010F021200406FFC506557EB8C23800C1\
10F50E00CD4600760E00CD46000E00CD4D00C3380076F53E\
01ED79F1C9F53E00ED79F1C9\0";
