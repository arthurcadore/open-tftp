cmake_minimum_required(VERSION 3.13)
project(tftpclient.out)

# Use g++ as the C++ compiler
set(CMAKE_CXX_COMPILER g++)

# Use C++14 standard explicitly
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define output directory for the binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)

# Create output directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Add the executable
add_executable(tftpclient.out 
    app/main.cpp 
    app/main.h 
    app/requisition.cpp 
    app/requisition.h 
    app/io.cpp 
    app/io.h
)

# Custom target for 'make run'
add_custom_target(run
    COMMAND tftpclient.out || echo "Execution failed!"
    DEPENDS tftpclient.out
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Enable verbose output for the build process
set(CMAKE_VERBOSE_MAKEFILE ON)
