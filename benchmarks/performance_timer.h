#ifndef PERFORMANCE_TIMER_H
#define PERFORMANCE_TIMER_H

#include <chrono>
#include <vector>

// Function to measure execution time of a BigUint operation
template <typename Func>
double measureExecutionTime(Func function, int iterations = 10) {
    std::vector<double> times;

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        function();  // Execute the function being benchmarked
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());
    }

    // Compute the average execution time
    double sum = 0;
    for (double time : times) {
        sum += time;
    }

    return sum / iterations;
}

#endif // PERFORMANCE_TIMER_H
