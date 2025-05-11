# Add executable for examples
add_executable(factorization main.cpp)

# Link the Crypto library to the example executable
target_link_libraries(factorization PRIVATE Crypto)

# Include the library headers in examples
target_include_directories(factorization PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Copy resources directory to binary dir
add_custom_command(
        TARGET factorization POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:factorization>/resources
)