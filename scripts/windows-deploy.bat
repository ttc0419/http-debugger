@echo off

set QT_PATH=C:\Qt
set QT_VERSION=6.3.0
set CRT_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\14.31.31103\x64\Microsoft.VC143.CRT
set CRT_LIBS=(vcruntime140_1.dll msvcp140_1.dll msvcp140_2.dll)

rd /s /q build

cmake "-DCMAKE_PREFIX_PATH=%QT_PATH%\%QT_VERSION%\msvc2019_64" -S .. -B build

cd build
MSBuild http-debugger.sln /property:Configuration=Release

cd ..
mkdir http-debugger
move build\Release\http-debugger.exe http-debugger
rd /s /q build

cd http-debugger
windeployqt --no-translations --no-opengl-sw --no-system-d3d-compiler http-debugger.exe
for %%l in %CRT_LIBS% do copy "%CRT_PATH%\%%l" .
cd ..

pause
