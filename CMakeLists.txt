cmake_minimum_required(VERSION 3.22)
project(Crypto)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check for MSVC
if(MSVC)
    message(STATUS "Using Microsoft Visual C++ compiler")

    # Enable useful MSVC flags
    add_compile_options(/W4 /WX) # Treat warnings as errors
else()
    message(WARNING "You are not using MSVC! This project is optimized for MSVC.")
endif()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(example)
