# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include_directories(${PROJECT_SOURCE_DIR}/src/)
project(Coremark-RP2040 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
file (GLOB all_SRCS "${PROJECT_SOURCE_DIR}/src/*.c" "${PROJECT_SOURCE_DIR}/src/*.h")
add_executable(Coremark-RP2040 ${all_SRCS})

pico_set_program_name(Coremark-RP2040 "Coremark-RP2040")
# pico_set_program_version(Coremark-RP2040 "1.0")

pico_enable_stdio_uart(Coremark-RP2040 0)
pico_enable_stdio_usb(Coremark-RP2040 1)

# Add the standard library to the build
target_link_libraries(Coremark-RP2040 pico_stdlib)

# Add any user requested libraries
target_link_libraries(Coremark-RP2040
        hardware_timer
        hardware_clocks
        )

pico_add_extra_outputs(Coremark-RP2040)

