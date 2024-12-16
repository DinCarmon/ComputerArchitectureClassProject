# Minimum CMake version
cmake_minimum_required(VERSION 3.10.0)

# Project name
project(ComputerArchitectureClassProject VERSION 0.1.0 LANGUAGES C)

# Specify the C standard to use
set(CMAKE_C_STANDARD 99)

cmake_minimum_required(VERSION 3.10)

project(MyCProject C)

# Set the C standard (adjust if needed)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set flags for maximum security and debugging
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    # Enable security features
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-all")  # Enable stack protection
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")                   # Position Independent Executable
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")                   # Position Independent Code
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat")                 # Enable format string checks
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wformat-security")        # Check for format string vulnerabilities
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")    # Suppress warnings about unknown pragmas
    
    # Enable warnings and treat them as errors
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")                    # Enable all warnings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")                  # Enable extra warnings
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")                  # Treat warnings as errors

    # Debugging flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")                       # Include debug information in the binary

    # Optimization flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")                      # Optimize for speed (without breaking debuggability)
    
    # Additional security-related flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GLIBCXX_ASSERTIONS")    # Enable runtime checks for C++ assertions
    
    # Enable sanitizers if you need to catch issues at runtime (optional)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")      # Address sanitizer
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")    # Undefined behavior sanitizer
endif()

# Add the include directory (header files)
include_directories(${CMAKE_SOURCE_DIR}/headers)

# Set the source files (all C files in the src directory)
set(SOURCE_FILES
    src/main.c
)

# Create an executable target from the source files
add_executable(ComputerArchitectureClassProjectExecutable ${SOURCE_FILES} "headers/cache.h")

