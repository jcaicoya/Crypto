# FetchContent module is required
include(FetchContent)

# Download and configure Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE  # ✅ Fix for CMake 3.28+ policy CMP0135
)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(CryptoTests BigUintTest.cpp)

# Link the Crypto library and Google Test
target_link_libraries(CryptoTests PRIVATE Crypto gtest gtest_main)

# Include the library headers
target_include_directories(CryptoTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ✅ Enable CTest for running tests
enable_testing()
add_test(NAME CryptoTests COMMAND CryptoTests)
