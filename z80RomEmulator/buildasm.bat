REM build all the assembly code "main" files in this directory
REM clean up before calling assembler 
del *.lst
del *.sym
del *.hex
del *.obj

REM call zxasm flashled2
REM call zxasm teststack
call zxasm memcheck
REM call zxasm memfill
REM call zxasm flashled
REM call zxasm add