#include <vector>
#include <random>
#include <algorithm>

#include <benchmark/benchmark.h>

std::vector<std::uint8_t> getRandomVector(int numElements) {
    std::vector<uint8_t> listOfNumbers;
    listOfNumbers.reserve(numElements);
    std::random_device r;
    std::mt19937 mersenneTwister(r());
    std::uniform_int_distribution<uint8_t> d(0);
    for (int i = 0; i < numElements; i++) {
        listOfNumbers.emplace_back(d(mersenneTwister));
    }
    return listOfNumbers;
}

static void SortedNumbers(benchmark::State& state) {
    auto numElements = 2000;
    std::vector<std::uint8_t> listOfNumbers = getRandomVector(numElements);
    // Code inside this loop is measured repeatedly
    std::sort(listOfNumbers.begin(), listOfNumbers.end());
    for (auto _ : state) {
        std::uint64_t lessThan128 = 0;
        for (auto number : listOfNumbers) {
            if (number < 128u) {
                lessThan128+= number;
            }
        }
        benchmark::DoNotOptimize(lessThan128);
    }
}
// Register the function as a benchmark
BENCHMARK(SortedNumbers);

static void UnsortedNumbers(benchmark::State& state) {
    auto listOfNumbers = getRandomVector(2000);
    for (auto _ : state) {
        std::uint64_t lessThan128 = 0;
        for (auto number : listOfNumbers) {
            if (number < 128u) {
                lessThan128 += number;
            }
        }
        benchmark::DoNotOptimize(lessThan128);
    }
}
BENCHMARK(UnsortedNumbers);

BENCHMARK_MAIN();