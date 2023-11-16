@echo off
if "%~1"=="" (
    echo Usage: %0 [filename]
    exit /b
)

set "file=%~1"
set "tempfile=temp.txt"
set "backslash=\"

REM Check if the input file exists
if not exist "%file%" (
    echo Input file does not exist.
    exit /b
)

REM Process each line in the input file
for /f "delims=" %%a in (%file%) do (
    echo %%a%backslash%>> %tempfile%
)

REM Replace the original file with the updated content
move /y %tempfile% %file% >nul

echo Backslashes added to the end of each line in %file%.