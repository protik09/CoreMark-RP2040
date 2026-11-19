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

if(NOT DEFINED PICO_PLATFORM)
    set(PICO_PLATFORM rp2040 CACHE STRING "Set the platform.")
endif()

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Add source files
include_directories(${PROJECT_SOURCE_DIR}/src/)
project(Coremark-RP2040 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
file (GLOB all_SRCS "${PROJECT_SOURCE_DIR}/src/*.c" "${PROJECT_SOURCE_DIR}/src/*.h")
add_executable(${PROJECT_NAME} ${all_SRCS})

# Report the real compiler flags in CoreMark's "Compiler flags" line
# (COMPILER_FLAGS in src/core_portme.h). CoreMark's reference ports pass them
# the same way, as FLAGS_STR. The define applies to the whole target, so every
# source file is still compiled with the same flags.
string(TOUPPER "${CMAKE_BUILD_TYPE}" COREMARK_BUILD_TYPE)
set(COREMARK_FLAGS_STR "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${COREMARK_BUILD_TYPE}}")
string(REGEX REPLACE " +" " " COREMARK_FLAGS_STR "${COREMARK_FLAGS_STR}")
string(STRIP "${COREMARK_FLAGS_STR}" COREMARK_FLAGS_STR)
target_compile_definitions(${PROJECT_NAME} PRIVATE "FLAGS_STR=\"${COREMARK_FLAGS_STR}\"")

pico_set_program_name(${PROJECT_NAME} "Coremark-RP2040")
# pico_set_program_version(Coremark-RP2040 "1.0")

pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Add pico_stdlib library which aggregates commonly used features
# Add any user requested libraries
target_link_libraries(${PROJECT_NAME}
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
target_link_options(${PROJECT_NAME} PRIVATE "LINKER:--wrap=main")

# Generate extra build files
pico_add_extra_outputs(${PROJECT_NAME})
