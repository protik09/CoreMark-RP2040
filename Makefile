# Change cleaning and moving script depending on OS
ifeq ($(OS),Windows_NT)     # Windows
	CLEAN_SCRIPT := @echo off && .\scripts\clean_win.bat
	MOVE_FILES := for %%f in (.\build\*.elf .\build\*.bin .\build\*.hex) do (if exist %%f move %%f artifacts_to_upload)

else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux) # Linux
		CLEAN_SCRIPT := chmod +x scripts/clean_lin.sh && ./scripts/clean_lin.sh
		MOVE_FILES := mv build/*.elf build/*.bin build/*.hex artifacts_to_upload
	endif
endif

all:
	mkdir build && mkdir artifacts_to_upload && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DPICO_PLATFORM=rp2040 && ninja
	$(MOVE_FILES)
.PHONY: all

clean:
	$(CLEAN_SCRIPT)
.PHONY: clean