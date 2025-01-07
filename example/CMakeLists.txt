# Add executable for examples
add_executable(CryptoExample main.cpp)

# Link the Crypto library to the example executable
target_link_libraries(CryptoExample PRIVATE Crypto)

# Include the library headers in examples
target_include_directories(CryptoExample PRIVATE ${PROJECT_SOURCE_DIR}/include)
