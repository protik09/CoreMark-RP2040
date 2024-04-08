#!/bin/bash

# Directories to be removed
dirs_to_remove=("CMakeFiles" "CMakeScripts" "build" "build" "generated")

# Files to be removed
files_to_remove=("cmake_install.cmake" "CMakeCache.txt" "CMakeLists.txt.user" "CMakeDoxygenDefaults.cmake" "CMakeDoxyfile.in" "*.map" "*.bin" "*.dis" "*.elf" "*.hex" "*.uf2")

# Function to remove directories and files
function cleanup {
    for dir in "${dirs_to_remove[@]}"; do
        find . -type d -name "$dir" -exec rm -rf {} +
    done

    for file in "${files_to_remove[@]}"; do
        find . -type f -name "$file" -exec rm -f {} +
    done
}

# Call the cleanup function
make clean || cleanup
