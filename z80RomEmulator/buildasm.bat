REM build all the assembly code "main" files in this directory
REM clean up before calling assembler 
del *.lst
del *.sym
del *.hex

call zxasm memcheck
REM call zxasm memfill
pause