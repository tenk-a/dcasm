::###############################################
@echo off
setlocal
pushd %~dp0
:: cmake ver.3.6.x  -> 306
:: cmake ver.3.31.x -> 331
set CMAKE_VER=331
call :Init

:: Get arguments.
:ARG_LOOP
  if "%1"=="" goto ARG_LOOP_EXIT
  call :CheckOpt %1
  shift
goto ARG_LOOP
:ARG_LOOP_EXIT

if "%Compiler%"=="" call :CheckCompiler
if "%Compiler%"=="" goto USAGE

call :SetGenerator

set "Bld=bld\%TgtDir%"
if "%Rebuild%"=="1" if exist "%Bld%" rmdir /s /q "%Bld%"
if not exist "%Bld%" mkdir "%Bld%"

cmake -G "%Gen%" %GenOpts% -B %Bld% .
cmake --build %Bld% %BldOpts%
cmake --install %Bld% --prefix "%CD%\bin\%TgtDir%"

goto END


::###############################################
:Init
set Compiler=
set Arch=
set BldTyp=Release
set forceMake=0
set Gen=
set GenOpts=
set BldOpts=
set TgtDir=
set Rebuild=
exit /b 0

::###############################################
:CheckOpt
if /I "%1"=="vc"       set Compiler=vc
if /I "%1"=="vc71"     set Compiler=vc71
if /I "%1"=="vc80"     set Compiler=vc80
if /I "%1"=="vc90"     set Compiler=vc90
if /I "%1"=="vc100"    set Compiler=vc100
if /I "%1"=="vc110"    set Compiler=vc110
if /I "%1"=="vc120"    set Compiler=vc120
if /I "%1"=="vc140"    set Compiler=vc140
if /I "%1"=="vc141"    set Compiler=vc141
if /I "%1"=="vc142"    set Compiler=vc142
if /I "%1"=="vc143"    set Compiler=vc143
if /I "%1"=="vc2003"   set Compiler=vc71
if /I "%1"=="vc2005"   set Compiler=vc80
if /I "%1"=="vc2008"   set Compiler=vc90
if /I "%1"=="vc2010"   set Compiler=vc100
if /I "%1"=="vc2012"   set Compiler=vc110
if /I "%1"=="vc2013"   set Compiler=vc120
if /I "%1"=="vc2015"   set Compiler=vc140
if /I "%1"=="vc2017"   set Compiler=vc141
if /I "%1"=="vc2019"   set Compiler=vc142
if /I "%1"=="vc2022"   set Compiler=vc143
if /I "%1"=="msys"     set Compiler=mingw
if /I "%1"=="msys2"    set Compiler=mingw
if /I "%1"=="mingw"    set Compiler=mingw
if /I "%1"=="clang"    set Compiler=clang
if /I "%1"=="gcc"      set Compiler=gcc
if /I "%1"=="watcom"   set Compiler=watcom
if /I "%1"=="ow"       set Compiler=watcom
if /I "%1"=="dmc"      set Compiler=dmc
if /I "%1"=="borland"  set Compiler=borland

if /I "%1"=="x86"      set Arch=Win32
if /I "%1"=="Win32"    set Arch=Win32
if /I "%1"=="x64"      set Arch=x64
if /I "%1"=="Win64"    set Arch=x64
if /I "%1"=="Arm"      set Arch=Arm
if /I "%1"=="Arm64"    set Arch=Arm64

if /I "%1"=="release"  set BldTyp=Release
if /I "%1"=="rel"      set BldTyp=Release
if /I "%1"=="debug"    set BldTyp=Debug
if /I "%1"=="deb"      set BldTyp=Debug

if /I "%1"=="make"     set forceMake=1
if /I "%1"=="nmake"    set forceMake=1

if /I "%1"=="Re"       set Rebuild=1
if /I "%1"=="Rebuild"  set Rebuild=1

exit /b 0


::###############################################
:CheckCompiler
if /I "%Compiler%"=="vc"    goto L_VCVER
@if /I not "%PATH:borland=%"=="%PATH%" set Compiler=borland
@if /I not "%PATH:dm\bin=%"=="%PATH%"  set Compiler=dmc
@if /I not "%PATH:dmc\bin=%"=="%PATH%" set Compiler=dmc
@if /I not "%PATH:WATCOM=%"=="%PATH%"  set Compiler=watcom
@if /I not "%PATH:mingw=%"=="%PATH%"   set Compiler=mingw
@if /I not "%PATH:msys32=%"=="%PATH%"  set Compiler=mingw
@if /I not "%PATH:msys64=%"=="%PATH%"  set Compiler=mingw
:L_VCVER
@if /I not "%PATH:Microsoft Visual Studio .NET 2003=%"=="%PATH%" set Compiler=vc71
@if /I not "%PATH:Microsoft Visual Studio 8=%"=="%PATH%"    set Compiler=vc80
@if /I not "%PATH:Microsoft Visual Studio 9.0=%"=="%PATH%"  set Compiler=vc90
@if /I not "%PATH:Microsoft Visual Studio 10.0=%"=="%PATH%" set Compiler=vc100
@if /I not "%PATH:Microsoft Visual Studio 11.0=%"=="%PATH%" set Compiler=vc110
@if /I not "%PATH:Microsoft Visual Studio 12.0=%"=="%PATH%" set Compiler=vc120
@if /I not "%PATH:Microsoft Visual Studio 14.0=%"=="%PATH%" set Compiler=vc140
@if /I not "%PATH:Microsoft Visual Studio\2017=%"=="%PATH%" set Compiler=vc141
@if /I not "%PATH:Microsoft Visual Studio\2019=%"=="%PATH%" set Compiler=vc142
@if /I not "%PATH:Microsoft Visual Studio\2022=%"=="%PATH%" set Compiler=vc143

if "%Arch%"=="" call :CheckArch
exit /b 0


::###############################################
:CheckArch
set VcVer=%Compiler%
if /I "%VcVer:~0,2%"=="vc" set /a "VcVer=%VcVer:~2%"
set /a VcVer=%VcVer%
@if "%VcVer%"=="143"      if /I not "%PATH:\bin\HostX64\x64=%"=="%PATH%" set Arch=x64
@if "%VcVer%"=="142"      if /I not "%PATH:\bin\HostX64\x64=%"=="%PATH%" set Arch=x64
@if "%VcVer%"=="141"      if /I not "%PATH:\bin\HostX64\x64=%"=="%PATH%" set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 14.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 13.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 12.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 11.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 10.0\VC\BIN\amd64=%"=="%PATH%" set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 9.0\VC\BIN\amd64=%"=="%PATH%"  set Arch=x64
@if /I not "%PATH:Microsoft Visual Studio 8\VC\BIN\amd64=%"=="%PATH%"    set Arch=x64
if /I "%Arch%"=="" set Arch=Win32
exit /b 0


::###############################################
:SetGenerator
if /I "%Compiler:~0,2%"=="vc" goto VcGenerator
:L_MAKE
if /I "%BldTyp%"=="Release"   set "GenOpts=%GenOpts% -DCMAKE_BUILD_TYPE=Release"
if /I "%BldTyp%"=="Debug"     set "GenOpts=%GenOpts% -DCMAKE_BUILD_TYPE=Debug"

set "TgtDir=%Compiler%-%Arch%-%BldTyp%"
if /I "%Compiler%"=="mingw"   set "Gen=MinGW Makefiles"
::if /I "%Compiler%"=="msys"  set "Gen=MSYS Makefiles"
if /I "%Compiler%"=="borland" set "Gen=Borland Makefiles"
if /I "%Compiler%"=="watcom"  set "Gen=Watcom WMake"
if /I "%Compiler%"=="clang"   goto L_MsysClangMAKE
if /I "%Compiler%"=="dmc"     goto L_DmcMAKE
exit /b 0

:L_MsysClangMAKE
set CC=clang.exe
set CXX=clang++.exe
set "Gen=MinGW Makefiles"
exit /b 0

:L_DmcMAKE
:: Need dmc-cc.exe : https://github.com/tenk-a/cc_for_dmc
set CC=dmc-cc.exe
set CXX=dmc-cc.exe
::set "Gen=Unix Makefiles"
set "Gen=MinGW Makefiles"
set "GenOpts=%GenOpts% -DCMAKE_C_COMPILER_WORKS=1 -DCMAKE_CXX_COMPILER_WORKS=1"
exit /b 0

:VcGenerator
set "TgtDir=%Compiler%-%Arch%"
set VcVer=%Compiler%
if /I "%VcVer:~0,2%"=="vc" set /a "VcVer=%VcVer:~2%"
set /a VcVer=%VcVer%
set "Gen=NMake Makefiles"
if %forceMake%==1 goto L_MAKE
if %VcVer% equ  60 if %CMAKE_VER% leq 305 set "Gen=Visual Studio 6"
if %VcVer% equ  70 if %CMAKE_VER% leq 305 set "Gen=Visual Studio 7"
if %VcVer% equ  71 if %CMAKE_VER% leq 308 set "Gen=Visual Studio 7.NET 2003"
if %VcVer% equ  80 if %CMAKE_VER% leq 311 set "Gen=Visual Studio 8 2005"
if %VcVer% equ 100 if %CMAKE_VER% leq 324 set "Gen=Visual Studio 10 2010"
if %VcVer% equ 110 if %CMAKE_VER% leq 327 set "Gen=Visual Studio 11 2012"
if %VcVer% geq  90 if %VcVer% leq 110 if %CMAKE_VER% leq 328 set "Gen=Visual Studio 9 2008"
if %VcVer% equ 120 if %CMAKE_VER% leq 330 set "Gen=Visual Studio 12 2013"
if %VcVer% equ 140 set "Gen=Visual Studio 14 2015"
if %VcVer% equ 141 set "Gen=Visual Studio 15 2017"
if %VcVer% equ 142 set "Gen=Visual Studio 16 2019"
if %VcVer% equ 143 set "Gen=Visual Studio 17 2022"
if "%Gen%"=="NMake Makefiles" goto L_MAKE

set BldOpts=--config %BldTyp%

if /I "%Arch%"=="Win32" goto L_SKIP_VC_X64

if %VcVer% geq 90 (
  if %VcVer% leq 141 (
    set "Gen=%Gen% Win64"
  ) else (
    set "GenOpts=-A %Arch% %GenOpts%"
  )
)
:L_SKIP_VC_X64
echo %Gen%
exit /b 0


::###############################################
:USAGE
@echo gen_bld [Compiler] [Arch] [Release/Debug]
@echo   Compiler vc80,vc90,vc100,vc110,vc120,vc140,vc141,vc142,vc143
@echo            msys watcom
@echo   Arch     Win32,x64


::###############################################
:END
popd
endlocal
