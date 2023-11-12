@echo off
setlocal enabledelayedexpansion

set "inputFile=memcheck.obj"
set "outputFile=memcheckstripped.txt"


if exist "%outputFile%" del "%outputFile%"

for /f "tokens=*" %%a in (%inputFile%) do (
    set "line=%%a"
    set "line=!line:~9,-2!"
    echo !line!>>%outputFile%
)

echo "First 9 characters and last 2 characters stripped from %inputFile% and saved to %outputFile%."

endlocal