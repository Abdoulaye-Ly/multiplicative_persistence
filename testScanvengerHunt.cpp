#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cassert>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Function Prototypes
vector<int> getDigits(long long n, int base);
int multiplicativePersistenceNaive(long long n, int base);
int multiplicativePersistenceOptimized(long long n, int base);
void benchmark(long long number, int base);
void runTests();

// Extract digits in a given base
vector<int> getDigits(long long n, int base) {
    vector<int> digits;
    while (n > 0) {
        digits.push_back(n % base);
        n /= base;
    }
    return digits;
}

// Naive method
int multiplicativePersistenceNaive(long long n, int base) {
    int steps = 0;
    while (n >= base) {
        vector<int> digits = getDigits(n, base);
        long long product = 1;
        for (int digit : digits) {
            product *= digit;
        }
        n = product;
        steps++;
    }
    return steps;
}

// Optimized method using memoization
unordered_map<long long, int> memo;
int multiplicativePersistenceOptimized(long long n, int base) {
    if (memo.count(n)) return memo[n];

    int steps = 0;
    while (n >= base) {
        if (n % base == 0) return memo[n] = steps + 1;
        vector<int> digits = getDigits(n, base);
        long long product = 1;
        for (int digit : digits) {
            product *= digit;
        }
        n = product;
        steps++;
    }
    return memo[n] = steps;
}

// Function to measure execution time
void benchmark(long long number, int base) {
    cout << "Testing Number: " << number << " in Base " << base << endl;

    auto start_naive = high_resolution_clock::now();
    int naive_result = multiplicativePersistenceNaive(number, base);
    auto end_naive = high_resolution_clock::now();

    auto start_opt = high_resolution_clock::now();
    int opt_result = multiplicativePersistenceOptimized(number, base);
    auto end_opt = high_resolution_clock::now();

    auto naive_time = duration_cast<microseconds>(end_naive - start_naive).count();
    auto opt_time = duration_cast<microseconds>(end_opt - start_opt).count();

    cout << "Naive Result: " << naive_result << " steps (" << naive_time << " μs)" << endl;
    cout << "Optimized Result: " << opt_result << " steps (" << opt_time << " μs)" << endl;
    cout << "--------------------------------------" << endl;

    assert(naive_result == opt_result); // Validate correctness
}

// Function to run various test cases
void runTests() {
    cout << "\n=== Running Algorithm Tests ===\n";

    // Test cases: {number, base, expected steps}
    vector<tuple<long long, int, int>> testCases = {
        {39, 10, 3},                // Expected: 3 steps
        {999, 10, 4},                // Expected: 4 steps
        {77, 10, 4},                 // Expected: 4 steps
        {277777788888899, 10, 11},  // Expected: 11 steps (hard case)
        {987654321, 10, 2},         // Expected: 2 steps
        {255, 2, 1},                // Binary
        {255, 16, 1},               // Hexadecimal
        {12345, 8, 3},              // Octal
        {0, 10, 0},                 // Edge case: 0
        {10, 10, 1},                // Edge case: 10 -> 0
        {1, 10, 0},                 // Edge case: single-digit
    };

    for (const auto& testCase : testCases) {
        long long number = get<0>(testCase);
        int base = get<1>(testCase);
        int expectedSteps = get<2>(testCase);

        // Input validation
        if (base < 2 || base > 16) {
            cout << "Invalid base: " << base << " (must be between 2 and 16)" << endl;
            continue;
        }

        if (number < 0) {
            cout << "Invalid number: " << number << " (must be non-negative)" << endl;
            continue;
        }

        // Run benchmark
        benchmark(number, base);

        // Validate expected steps
        int naiveSteps = multiplicativePersistenceNaive(number, base);
        int optimizedSteps = multiplicativePersistenceOptimized(number, base);
        assert(naiveSteps == expectedSteps);
        assert(optimizedSteps == expectedSteps);
    }

    cout << "All tests passed successfully!\n";
}

int main() {
    runTests();
    return 0;
}