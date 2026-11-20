# Builds one UF2 per chip, each in its own build directory:
#   make all     Coremark-RP2040.uf2 and Coremark-RP2350.uf2 (needs the Arm GCC toolchain)
#   make riscv   Coremark-RP2350-RISCV.uf2 (needs the RISC-V GCC toolchain)
# The .uf2, .elf and .hex files are copied to artifacts_to_upload.

BUILD_TYPE := Debug

# Change cleaning script and CMake generator depending on OS
ifeq ($(OS),Windows_NT)     # Windows
	CLEAN_SCRIPT := @echo off && .\scripts\clean_win.bat
	GENERATOR := -G Ninja
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux) # Linux
		CLEAN_SCRIPT := chmod +x scripts/clean_lin.sh && ./scripts/clean_lin.sh
	endif
endif

# $(call build,<build dir>,<output name>,<cmake options>)
define build
	cmake -S . -B build/$(1) $(GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(3)
	cmake --build build/$(1) --parallel
	cmake -E make_directory artifacts_to_upload
	cmake -E copy build/$(1)/$(2).uf2 build/$(1)/$(2).elf build/$(1)/$(2).hex artifacts_to_upload
endef

all: rp2040 rp2350
.PHONY: all

rp2040:
	$(call build,rp2040,Coremark-RP2040,-DPICO_BOARD=pico)
.PHONY: rp2040

rp2350:
	$(call build,rp2350,Coremark-RP2350,-DPICO_BOARD=pico2)
.PHONY: rp2350

riscv:
	$(call build,rp2350-riscv,Coremark-RP2350-RISCV,-DPICO_BOARD=pico2 -DPICO_PLATFORM=rp2350-riscv)
.PHONY: riscv

clean:
	$(CLEAN_SCRIPT)
.PHONY: clean
