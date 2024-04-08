@echo off
setlocal

:: Directories to be removed
set "dirs_to_remove=CMakeFiles CMakeScripts build generated artifacts_to_upload"

:: Files to be removed
set "files_to_remove=cmake_install.cmake CMakeCache.txt CMakeLists.txt.user CMakeDoxygenDefaults.cmake CMakeDoxyfile.in *.ninja* *.map *.bin *.dis *.elf *.hex *.uf2 *.vcxproj *.filters *.sln"

:: Function to remove directories and files
for %%d in (%dirs_to_remove%) do (
    for /r . %%D in (%%d) do (
        if exist "%%D" (
            echo Removing directory "%%D"
            rd /s /q "%%D"
        )
    )
)

for %%f in (%files_to_remove%) do (
    for /r . %%F in (%%f) do (
        if exist "%%F" (
            echo Removing file "%%F"
            del /f /q "%%F"
        )
    )
)

endlocal
