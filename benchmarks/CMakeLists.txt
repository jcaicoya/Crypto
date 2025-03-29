# Benchmark: Arithmetic operations
add_executable(BenchmarkArithmetic benchmark_arithmetic.cpp)
target_link_libraries(BenchmarkArithmetic PRIVATE Crypto)
target_include_directories(BenchmarkArithmetic PRIVATE ${PROJECT_SOURCE_DIR}/benchmarks)

# Benchmark: Multiplication comparisons
add_executable(BenchmarkMultiplication benchmark_multiplication.cpp)
target_link_libraries(BenchmarkMultiplication PRIVATE Crypto)
target_include_directories(BenchmarkMultiplication PRIVATE ${PROJECT_SOURCE_DIR}/benchmarks)
