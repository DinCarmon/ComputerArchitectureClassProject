# Minimum CMake version
cmake_minimum_required(VERSION 3.10.0)

# Project name
project(ComputerArchitectureClassProjectAssembler VERSION 0.1.0 LANGUAGES C)

cmake_minimum_required(VERSION 3.10)

# Set the C standard (adjust if needed)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_D)

# Set the source files (all C files in the src directory)
set(SOURCE_FILES
    asm.c
)

# Create an executable target from the source files
add_executable(ComputerArchitectureClassProjectAssembler ${SOURCE_FILES})
