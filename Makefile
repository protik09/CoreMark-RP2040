.PHONY: all clean

ifeq ($(OS),Windows_NT)     # Windows
    CLEAN_SCRIPT := @echo off && .\scripts\clean_win.bat
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux) # Linux
        CLEAN_SCRIPT := chmod +x scripts/clean_lin.sh && ./scripts/clean_lin.sh
    endif
endif

all:
	cmake .
	ninja

clean:
	$(CLEAN_SCRIPT)
