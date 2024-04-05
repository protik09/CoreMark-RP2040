ifeq ($(OS),Windows_NT)     # Windows
    CLEAN_SCRIPT := .\clean_win.bat
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux) # Linux
        CLEAN_SCRIPT := ./clean_lin.sh
    endif
endif

clean:
	$(CLEAN_SCRIPT)

all:
	cmake .
	ninja

