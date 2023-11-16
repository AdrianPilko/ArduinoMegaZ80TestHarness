REM build all the assembly code "main" files in this directory
REM clean up before calling assembler 
del *.lst
del *.sym
del *.hex
del *.obj

set "base_filename=memcheck"

call zxasm %base_filename%

REM Set the filename as a variable
REM Example variable containing the base filename


REM Concatenate the base filename with .obj to create a new filename
set "new_filename=%base_filename%.obj"

echo object filename: %new_filename%
call addFormatForProgFile.bat %new_filename%


pause
