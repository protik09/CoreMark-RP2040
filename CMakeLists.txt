# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.12)

# Set C/C++ Standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

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

# Generate extra build files
pico_add_extra_outputs(${PROJECT_NAME})
