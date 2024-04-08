@echo OFF

:: C:\WINDOWS\system32\cmd.exe /k "%PICO_INSTALL_PATH%\pico-env.cmd"
"%PICO_INSTALL_PATH%\pico-env.cmd"
IF ERRORLEVEL 1 (
    echo Add PICO_INSTALL_PATH to Windows Environment Variables. For example: PICO_INSTALL_PATH = "C:\Program Files\Raspberry Pi\Pico SDK v1.5.1"
    exit /b %ERRORLEVEL%
)
