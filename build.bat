::
::  (C) 2016 by Remo Dentato (rdentato@gmail.com)
:: 
:: This software is distributed under the terms of the MIT license:
::  https://opensource.org/licenses/MIT   
::     
::                     ___   __
::                  __/  /_ /  )
::          ___  __(_   ___)  /
::         /  / /  )/  /  /  /  Minimalist
::        /  (_/  //  (_ /  /  C utility 
::       (____,__/(_____(__/  Library
:: 

:: [[[
:: # Building `clibutl` on Windows
::
::   The development environment for `clibutl` is the GCC toolchain (gcc, make, ...).
:: To start using it with other compilers (and on different SO) you don't need to
:: recompile anything, you just add the `utl.h` and `utl.c` files to your project.
:: However, you may want to rebuild (and test) `clibutl` with your compiler to get a
:: better understanding of how it works and to ensure it is fully compatible with 
:: your chosen tool it using a different compiler.
::
:: This script builds the header and the C files, compiles all the tests and runs them
::
:: ]]]

@echo off
echo Start build > build.log

if "%1"=="clean" goto clean
if "%1"=="test"  goto test
if "%1"=="MSC"   goto MSC
if "%1"=="PCC"   goto PCC
if "%1"=="PCC64" goto PCC64

:usage
echo Usage: build opt
echo   compilers: MSC     Microsoft C    (VS2015)
echo              PCC     Pelles C 32bit (8.0)
echo              PCC64   Pelles C 64bit (8.0)
echo    commands: clean   cleanup
echo              test    run tests (no build)
goto theend

:clean

del /Q /F src\*.obj  2> nul
del /Q /F src\*.exe  2> nul
del /Q /F src\utl.c  2> nul
del /Q /F src\utl.h  2> nul
del /Q /F test\*.obj 2> nul
del /Q /F test\*.exe 2> nul
del /Q /F test\*.log 2> nul
del /Q /F test\*.tmp 2> nul
del /Q /F dist\utl.c 2> nul
del /Q /F dist\utl.h 2> nul
del /Q /F build.log  2> nul
goto theend

:MSC
echo Compiling with Microsoft C/C++ (cl)

set BUILD_1=cl /O2 /nologo
set BUILD_2=
set BUILD_O=/Fe
set BUILD_C=/c 
goto compile

:PCC
echo Compiling with Pelles C (32bit)
set BUILD_1=cc /Tx86-coff /O2 /std:C99
set BUILD_2="%PellesCDir%\lib\win\kernel32.lib"
set BUILD_O=/o
set BUILD_C=/c 
goto compile

:PCC64
echo Compiling with Pelles C (32bit)
set BUILD_1=cc /Tamd64-coff /O2 /std:C99
set BUILD_2=
set BUILD_O=/o
set BUILD_C=/c 
goto compile

:compile

:: Create the utl.h and utl.c files

cd src

type utl_hdr.h utl_log.h utl_mem.h utl_vec.h utl_pmx.h utl_fsm.h utl_end.h > utl.h 2>> ..\build.log
type utl_hdr.c utl_log.c utl_mem.c utl_vec.c utl_pmx.c > utl.c 2>> ..\build.log
copy utl.? ..\dist >>..\build.log



%BUILD_1% utl_unc.c %BUILD_2% %BUILD_O%utl_unc.exe >> ../build.log
%BUILD_1% %BUILD_C% utl.c >> ../build.log
copy utl.obj ..\test >> ../build.log
copy utl.h ..\test >> ../build.log

cd ..\test
%BUILD_1% ut_utf.c  %BUILD_2% utl.obj %BUILD_O%t_utf.exe   >> ../build.log
%BUILD_1% ut_buf.c  %BUILD_2% utl.obj %BUILD_O%t_buf.exe   >> ../build.log
%BUILD_1% ut_mem.c  %BUILD_2% utl.obj %BUILD_O%t_mem.exe   >> ../build.log
%BUILD_1% ut_pmx.c  %BUILD_2% utl.obj %BUILD_O%t_pmx.exe   >> ../build.log
%BUILD_1% ut_pmx2.c %BUILD_2% utl.obj %BUILD_O%t_pmx2.exe  >> ../build.log
%BUILD_1% ut_pmx3.c %BUILD_2% utl.obj %BUILD_O%t_pmx3.exe  >> ../build.log
%BUILD_1% ut_pmx4.c %BUILD_2% utl.obj %BUILD_O%t_pmx4.exe  >> ../build.log      
%BUILD_1% ut_utf.c  %BUILD_2% utl.obj %BUILD_O%t_utf.exe   >> ../build.log      
%BUILD_1% ut_vec.c  %BUILD_2% utl.obj %BUILD_O%t_vec.exe   >> ../build.log
%BUILD_1% ut_logassert.c  %BUILD_2% utl.obj %BUILD_O%t_logassert.exe   >> ../build.log
del utl.h
cd ..

:test
cd test
echo Start tests
for %%f in (t_???.exe) do %%f
for %%f in (t_????.exe) do %%f
echo ut_logassert.exe is expected to abort and return an error
t_logassert
find "#KO:" l_*.log

cd ..

:theend
