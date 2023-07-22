@echo off
setlocal enabledelayedexpansion

:: default arguments:
::   script/test_conan.bat array19
if "%~1"=="" (
  set ConanCase=array19
) else (
  set ConanCase=%1
)

set "BUILD_PATH=build/%ConanCase%"
set "SOURCE_PATH=conan/tests/%ConanCase%"
set "BASE_DIR=%~dp0.."

pushd "%~dp0.."

mkdir "%BUILD_PATH%"
cd "%BUILD_PATH%"

conan install "%BASE_DIR%/%SOURCE_PATH%" %2 %3 %4 %5
if !errorlevel! neq 0 exit /b !errorlevel!

cmake "%BASE_DIR%/%SOURCE_PATH%" "-DCMAKE_BUILD_TYPE=Release" -G "Ninja"
if !errorlevel! neq 0 exit /b !errorlevel!

cmake --build .
if !errorlevel! neq 0 exit /b !errorlevel!

ctest
if !errorlevel! neq 0 exit /b !errorlevel!

popd
