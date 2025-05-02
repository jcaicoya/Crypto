# FetchContent module is required
include(FetchContent)

# Download and configure Google Test
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(CryptoTests BigUintTest.cpp)

# Link the Crypto library and Google Test
target_link_libraries(CryptoTests PRIVATE gtest gtest_main Crypto)

# Include the library headers
target_include_directories(CryptoTests PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ✅ Enable CTest for running tests
enable_testing()
add_test(NAME CryptoTests COMMAND CryptoTests)
