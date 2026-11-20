@echo OFF

:: Sets up this CMD window for building CoreMark: PICO_SDK_PATH, the Arm and
:: RISC-V compilers, CMake, Ninja and picotool.
::
:: It uses what the Raspberry Pi Pico VS Code extension installs in
:: %USERPROFILE%\.pico-sdk (SDK 2.x, which the RP2350 needs). If there is no
:: such folder it falls back to the old pico-setup-windows installer
:: (PICO_INSTALL_PATH), which stops at SDK 1.5.1 and only builds for the RP2040.
::
:: Run it from CMD in the project directory. The settings last until the
:: window is closed.

set "PICO_ROOT=%USERPROFILE%\.pico-sdk"
if not exist "%PICO_ROOT%\sdk\" goto :old_installer

:: SDK: keep PICO_SDK_PATH if it is already set, else take the newest one.
if not defined PICO_SDK_PATH call :newest_dir PICO_SDK_PATH "%PICO_ROOT%\sdk"
if not defined PICO_SDK_PATH goto :not_found
for %%D in ("%PICO_SDK_PATH%") do set "PICO_SDK_VERSION=%%~nxD"

:: Tools: the newest folder that holds the program.
set "ARM_GCC_BIN="
set "RISCV_GCC_BIN="
set "CMAKE_BIN="
set "NINJA_BIN="
set "picotool_DIR="
for /d %%D in ("%PICO_ROOT%\toolchain\*") do if exist "%%~fD\bin\arm-none-eabi-gcc.exe" set "ARM_GCC_BIN=%%~fD\bin"
for /d %%D in ("%PICO_ROOT%\toolchain\*") do if exist "%%~fD\bin\riscv32-pico-elf-gcc.exe" set "RISCV_GCC_BIN=%%~fD\bin"
for /d %%D in ("%PICO_ROOT%\toolchain\*") do if exist "%%~fD\bin\riscv32-unknown-elf-gcc.exe" set "RISCV_GCC_BIN=%%~fD\bin"
for /d %%D in ("%PICO_ROOT%\cmake\*") do if exist "%%~fD\bin\cmake.exe" set "CMAKE_BIN=%%~fD\bin"
for /d %%D in ("%PICO_ROOT%\ninja\*") do if exist "%%~fD\ninja.exe" set "NINJA_BIN=%%~fD"
:: picotool must be at least the SDK's version. Prefer the same version.
for /d %%D in ("%PICO_ROOT%\picotool\*") do if exist "%%~fD\picotool\picotool.exe" set "picotool_DIR=%%~fD\picotool"
if exist "%PICO_ROOT%\picotool\%PICO_SDK_VERSION%\picotool\picotool.exe" set "picotool_DIR=%PICO_ROOT%\picotool\%PICO_SDK_VERSION%\picotool"

if not defined ARM_GCC_BIN (
    echo ERROR: No Arm GCC found in "%PICO_ROOT%\toolchain".
    echo Open or create a Pico project in the VS Code extension once so it downloads the tools.
    exit /b 1
)

if defined NINJA_BIN set "PATH=%NINJA_BIN%;%PATH%"
if defined CMAKE_BIN set "PATH=%CMAKE_BIN%;%PATH%"
if defined RISCV_GCC_BIN set "PATH=%RISCV_GCC_BIN%;%PATH%"
set "PATH=%ARM_GCC_BIN%;%PATH%"
:: cmake would otherwise pick a Visual Studio generator on Windows.
set "CMAKE_GENERATOR=Ninja"

echo PICO_SDK_PATH = %PICO_SDK_PATH%
echo Arm GCC       = %ARM_GCC_BIN%
if defined RISCV_GCC_BIN (echo RISC-V GCC    = %RISCV_GCC_BIN%) else (echo RISC-V GCC    = not found: "make riscv" won't work, the other builds will)
if defined CMAKE_BIN (echo CMake         = %CMAKE_BIN%) else (echo CMake         = not in %PICO_ROOT%, using the one on PATH)
if defined NINJA_BIN (echo Ninja         = %NINJA_BIN%) else (echo Ninja         = not in %PICO_ROOT%, using the one on PATH)
if defined picotool_DIR (echo picotool      = %picotool_DIR%) else (echo picotool      = not found: the SDK will try to build it, which needs a host C compiler)
exit /b 0

:old_installer
if not defined PICO_INSTALL_PATH goto :not_found
call "%PICO_INSTALL_PATH%\pico-env.cmd"
IF ERRORLEVEL 1 goto :not_found
echo.
echo NOTE: pico-setup-windows provides Pico SDK 1.x, which can only build the RP2040 UF2.
echo       Install the Raspberry Pi Pico VS Code extension for the RP2350 builds.
exit /b 0

:not_found
echo No Pico SDK found.
echo Install the Raspberry Pi Pico extension for VS Code. It puts SDK 2.x and its tools in "%PICO_ROOT%".
echo (The older pico-setup-windows installer is still used if PICO_INSTALL_PATH is set, but it only builds for the RP2040.)
exit /b 1

:: :newest_dir <variable> <folder> - sets <variable> to the last subfolder of <folder> in name order
:newest_dir
for /d %%D in ("%~2\*") do set "%~1=%%~fD"
exit /b 0
