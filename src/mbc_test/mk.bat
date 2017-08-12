@echo off
rem
rem mk.bat [COMPILER] [debug|release] [tagetname]
rem
setlocal
set COMPILER=%1
set MOD=%2

rem アプリごとの設定.
set TGT=test
set SRCS=test.cpp test_mbc.cpp ../mbc.c
set ERRFILE=err.txt

if not "%3"=="" (
  set TGT=%3
)

rem set EXENAME=%TGT%_%COMPILER%.exe
set EXENAME=%TGT%.exe

rem コンパイル環境設定.

if "%COMPILER%"=="" set COMPILER=vc

if /i "%COMPILER%"=="vc"      goto J_VC
if /i "%COMPILER%"=="vc64"    goto J_VC
if /i "%COMPILER%"=="cl"      goto J_VC
if /i "%COMPILER%"=="gcc"     goto J_GCC
if /i "%COMPILER%"=="g++"     goto J_GCC
if /i "%COMPILER%"=="mingw"   goto J_GCC
if /i "%COMPILER%"=="borland" goto J_BCC
if /i "%COMPILER%"=="bcc"     goto J_BCC
if /i "%COMPILER%"=="dm"      goto J_DMC
if /i "%COMPILER%"=="dmc"     goto J_DMC
if /i "%COMPILER%"=="ow"      goto J_WATCOM
if /i "%COMPILER%"=="watcom"  goto J_WATCOM
if /i "%COMPILER%"=="wat"     goto J_WATCOM
if /i "%COMPILER%"=="lccwin"  goto J_LCCWIN
if /i "%COMPILER%"=="lcc"     goto J_LCC
if /i "%COMPILER%"=="clean"   goto J_CLEAN

echo コンパイラが指定されていない.
goto :EOF

:J_VC
  set CC=cl
  set O_CMN=-W4 -wd4127 -wd4996 -wd4244 -GR- -EHac -D_CRT_SECURE_NO_WARNINGS
  set O_REL=-Ox -DNDEBUG -MT
  set O_DEB=-ZI -MTd
  set O_OUT=-Fe
  set LIBS=
  set ERN=1
  goto COMP

:J_GCC
  set CC=g++
  set O_CMN=-Wall --input-charset=cp932 --exec-charset=cp932 
  set O_REL=-O2 -DNDEBUG
  set O_DEB=-g
  set O_OUT=-o 
  set LIBS=
  set ERN=2
  goto COMP

:J_BCC
  set CC=bcc32
  set O_CMN=-w -w-8004 -w-8057 -w-8066 -w-8071 -w-8026 -w-8027
  set O_REL=-O2 -DNDEBUG -Ox
  set O_DEB=-v
  set O_OUT=-e
  set LIBS=
  set ERN=1
  goto COMP

:J_DMC
  set CC=dmc
  set O_CMN=-w -Ae -j0 -DUNUSE_SJIS
  set O_REL=-o -DNDEBUG
  set O_DEB=-g
  set O_OUT=-o
  set LIBS=
  set ERN=1
  goto COMP

:J_WATCOM
  set CC=owcc
  set O_CMN=-Wall -feh -DUNUSE_SJIS
  set O_REL=-Ot -O3 -DNDEBUG
  set O_DEB=-gcodeview
  set O_OUT=-o 
  set LIBS=
  set ERN=1
  goto COMP

:J_LCCWIN
  set CC=lcc
  set O_CMN=-A
  set O_REL=-O -DNDEBUG
  set O_DEB=-g5
  set O_OUT=-Fo
  set LIBS=
  set ERN=1
  goto COMP

:J_CLEAN
  del *.obj
  del *.tds
  del *.o
  del *.bak
  del *.ncb
  del *.ilk
  del *.suo
  del *.err
  del *.tmp
  goto :EOF

rem 実際のコンパイル
:COMP
if "%MOD%"=="debug" (
  set OPTS=%O_DEB% %O_CMN% %OPTS_ADD%
) else (
  set OPTS=%O_REL% %O_CMN% %OPTS_ADD%
)
%CC% %OPTS% %O_OUT%%EXENAME% %SRCS% %LIBS% %ERN%>%ERRFILE%
if errorlevel 1 (
  type %ERRFILE%
  pause
) else (
  type %ERRFILE%
)
