REM build all the assembly code "main" files in this directory
REM clean up before calling assembler 
del *.lst
del *.sym
del *.hex
del *.obj

set "base_filename=byteForeverROM"

call zxasm %base_filename%


call python convertOBJToBIN.py ./byteForeverROM.obj
