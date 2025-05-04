#ifndef PERFORMANCE_TIMER_H
#define PERFORMANCE_TIMER_H

#include <chrono>
#include <vector>

// Function to measure execution time of a BigUint operation
template <typename Func>
double measureExecutionTime(Func function, const int iterations = 30) {
    std::vector<double> times;

    for (int ii = 0; ii < iterations; ii++) {
        auto start = std::chrono::high_resolution_clock::now();
        function();  // Execute the function being benchmarked
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        times.push_back(duration.count());
    }

    // Compute the average execution time
    double totalTime = 0;
    for (const auto time : times) {
        totalTime += time;
    }

    return totalTime / iterations;
}

#endif // PERFORMANCE_TIMER_H
