@echo off
rem Parameter %1 is the input file fully qualified with drive letter
rem Parameter %2 is the output file fully qualified with drive letter
rem Build the arm header files for Mono for Vita
if not exist c:\cygwin\bin\bash.exe goto nocygwin
pushd
cd /d %~dp1
d2u -q -n %1 %~n1.tmp
if ERRORLEVEL 1 goto fail
bash %~n1.tmp > %2
del %~n1.tmp
popd
goto end
:fail
popd
@echo If you're getting the error "sed: Command not found" or
@echo "d2u not recognized" then you need to add C:\cygwin\bin
@echo to your path
goto end
:nocygwin
@echo Cygwin was not detected. Install Cygwin from http://www.cygwin.com
pause
start http://www.cygwin.com
:end