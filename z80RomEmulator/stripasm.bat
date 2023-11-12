@echo off
REM Make sure you set the input output files properly to what you have

setlocal enabledelayedexpansion

set "inputFile=flashled2.obj"
set "outputFile=flashled2.txt"

if exist "%outputFile%" del "%outputFile%"

for /f "tokens=*" %%a in (%inputFile%) do (
    set "line=%%a"
    set "line=!line:~9,-2!"
    echo !line!>>%outputFile%
)

echo "First 9 characters and last 2 characters stripped from %inputFile% and saved to %outputFile%."

endlocal