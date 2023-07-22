@echo off
setlocal enabledelayedexpansion

:: default arguments:
::   script/test_cmake.bat msvc debug
if "%~1"=="" (
  set ConfigPreset=msvc
) else (
  set ConfigPreset=%1
)
if "%~2"=="" (
  set Config=debug
) else (
  set Config=%2
)
set "BuildPreset=%ConfigPreset%-%Config%"
set "TestPreset=test-%BuildPreset%"

set "BUILD_PATH=build/%ConfigPreset%"

pushd "%~dp0.."
if not exist "%BUILD_PATH%/compile_commands.json" (
  cmake -S . --preset "%ConfigPreset%"
  if !errorlevel! neq 0 exit /b !errorlevel!
)
cmake --build --preset "%BuildPreset%"
if !errorlevel! neq 0 exit /b !errorlevel!

ctest --preset "%TestPreset%"
if !errorlevel! neq 0 exit /b !errorlevel!

popd
