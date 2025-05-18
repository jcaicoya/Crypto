add_executable(factorization factorization-main.cpp)
target_link_libraries(factorization PRIVATE Crypto)
target_include_directories(factorization PRIVATE ${PROJECT_SOURCE_DIR}/include)

option(USE_SOURCE_RESOURCE_PATH "Use source resource directory instead of copying it" ON)
if (USE_SOURCE_RESOURCE_PATH)
    message("Using source resources directory")
    target_compile_definitions(factorization PRIVATE RSC_PATH="${CMAKE_SOURCE_DIR}/resources")
else ()
    message("Copy resources directory to binary dir")
    add_custom_command(
        TARGET factorization POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:factorization>/resources
    )
endif ()