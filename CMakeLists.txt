# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Set the build type.")
endif()

# Chip selection. Use a separate build directory for each one:
#   RP2040, Pico / Pico W (default)   cmake ..
#   RP2350 Arm, Pico 2 / Pico 2 W     cmake .. -DPICO_BOARD=pico2
#   RP2350 RISC-V, Pico 2 / Pico 2 W  cmake .. -DPICO_BOARD=pico2 -DPICO_PLATFORM=rp2350-riscv
# The SDK takes the platform from PICO_BOARD, and with neither set it builds
# for the RP2040 (Pico). The RP2350 needs Pico SDK 2.0 or later.

# Run each chip at its fastest officially supported clock, for the best score:
# 200 MHz on the RP2040 (the SDK raises the core voltage to 1.15 V for it) and
# 150 MHz on the RP2350 (its default). This needs Pico SDK 2.1.1 or later; older
# SDKs keep their default clocks. Pass -DPICO_USE_FASTEST_SUPPORTED_CLOCK=0 for
# the SDK's default clocks (125 MHz on the RP2040).
if(NOT DEFINED PICO_USE_FASTEST_SUPPORTED_CLOCK)
    set(PICO_USE_FASTEST_SUPPORTED_CLOCK 1)
endif()

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Each chip gets its own file name (Coremark-RP2040.uf2, Coremark-RP2350.uf2,
# Coremark-RP2350-RISCV.uf2), so a UF2 can't be flashed onto the wrong board.
if(PICO_PLATFORM STREQUAL "rp2040")
    set(COREMARK_TARGET Coremark-RP2040)
elseif(PICO_PLATFORM STREQUAL "rp2350-arm-s")
    set(COREMARK_TARGET Coremark-RP2350)
elseif(PICO_PLATFORM STREQUAL "rp2350-riscv")
    set(COREMARK_TARGET Coremark-RP2350-RISCV)
else()
    message(FATAL_ERROR "CoreMark: unsupported PICO_PLATFORM '${PICO_PLATFORM}'. "
                        "Use rp2040, rp2350 (Arm) or rp2350-riscv.")
endif()

# Add source files
include_directories(${PROJECT_SOURCE_DIR}/src/)
project(Coremark-RP2040 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
file (GLOB all_SRCS "${PROJECT_SOURCE_DIR}/src/*.c" "${PROJECT_SOURCE_DIR}/src/*.h")
add_executable(${COREMARK_TARGET} ${all_SRCS})

# Report the real compiler flags in CoreMark's "Compiler flags" line
# (COMPILER_FLAGS in src/core_portme.h). CoreMark's reference ports pass them
# the same way, as FLAGS_STR. The define applies to the whole target, so every
# source file is still compiled with the same flags.
string(TOUPPER "${CMAKE_BUILD_TYPE}" COREMARK_BUILD_TYPE)
set(COREMARK_FLAGS_STR "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${COREMARK_BUILD_TYPE}}")
string(REGEX REPLACE " +" " " COREMARK_FLAGS_STR "${COREMARK_FLAGS_STR}")
string(STRIP "${COREMARK_FLAGS_STR}" COREMARK_FLAGS_STR)
target_compile_definitions(${COREMARK_TARGET} PRIVATE "FLAGS_STR=\"${COREMARK_FLAGS_STR}\"")

pico_set_program_name(${COREMARK_TARGET} "${COREMARK_TARGET}")
# pico_set_program_version(${COREMARK_TARGET} "1.0")

pico_enable_stdio_uart(${COREMARK_TARGET} 0)
pico_enable_stdio_usb(${COREMARK_TARGET} 1)

# Add pico_stdlib library which aggregates commonly used features
# Add any user requested libraries
target_link_libraries(${COREMARK_TARGET}
        pico_stdlib
        pico_multicore
        hardware_timer
        hardware_clocks
        )

# CoreMark's main() (src/core_main.c, unmodified) is run once per core
# configuration: core 0 only, core 1 only, then both cores. With --wrap=main
# the SDK start-up code calls __wrap_main() in src/core_portme.c, which calls
# the real main() once per run. This is a link option only, so every source
# file is still compiled with the same flags.
target_link_options(${COREMARK_TARGET} PRIVATE "LINKER:--wrap=main")

# Generate extra build files
pico_add_extra_outputs(${COREMARK_TARGET})
