@echo off
pushd ..
call vendor\premake5\premake5.exe vs2019
popd
pause
