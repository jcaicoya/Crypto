# Add library
add_library(Crypto STATIC BigUint.cpp
        ../benchmarks/benchmark_multiplication.cpp
        Primality.cpp)

# Include directory for the library
target_include_directories(Crypto PUBLIC ${PROJECT_SOURCE_DIR}/include)
