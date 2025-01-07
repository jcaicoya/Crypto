# FetchContent module is required
include(FetchContent)

# Download and configure Google Test
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.tar.gz
)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(CryptoTests BigIntTest.cpp)

# Link the Crypto library and Google Test
target_link_libraries(CryptoTests PRIVATE Crypto gtest gtest_main)

# Include the library headers
target_include_directories(CryptoTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Add tests
add_test(NAME CryptoTests COMMAND CryptoTests)
