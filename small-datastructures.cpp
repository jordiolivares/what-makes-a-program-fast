#include <vector>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <benchmark/benchmark.h>

template <typename T>
std::vector<T> getRandomVector(int numElements) {
    std::vector<T> listOfNumbers;
    listOfNumbers.reserve(numElements);
    std::random_device r;
    std::mt19937 mersenneTwister(r());
    std::uniform_int_distribution<T> d(0);
    for (int i = 0; i < numElements; i++) {
        listOfNumbers.emplace_back(d(mersenneTwister));
    }
    return listOfNumbers;
}

template <typename T>
std::set<T> getRandomSet(int numElements) {
    auto listOfNumbers = getRandomVector<T>(numElements);
    return std::set<T>(listOfNumbers.begin(), listOfNumbers.end());
}

template <typename T>
std::unordered_set<T> getRandomUnorderedSet(int numElements) {
    auto listOfNumbers = getRandomVector<T>(numElements);
    return std::unordered_set<T>(listOfNumbers.begin(), listOfNumbers.end());
}

template <typename BeginIt, typename EndIt>
std::vector<typename std::iterator_traits<BeginIt>::value_type> getShuffledData(BeginIt &&beginIt, EndIt &&endIt) {
    std::vector<typename std::iterator_traits<BeginIt>::value_type> listOfCandidates(std::forward<BeginIt>(beginIt), std::forward<EndIt>(endIt));
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(listOfCandidates.begin(), listOfCandidates.end(), g);
    listOfCandidates.resize(std::min(std::distance(beginIt, endIt), 50l));
    return listOfCandidates;
}

using DataType = std::uint8_t;
static constexpr auto NumElements = 20;

static void VectorStructure(benchmark::State& state) {
    auto listOfNumbers = getRandomVector<DataType>(NumElements);
    auto listOfCandidates = getShuffledData(listOfNumbers.begin(), listOfNumbers.end());
    for (auto _ : state) {
        for (auto candidate : listOfCandidates) {
            bool found = false;
            for (auto number : listOfNumbers) {
                if (number == candidate) {
                    found = true;
                    break;
                }
            }
            benchmark::DoNotOptimize(found);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(VectorStructure);

static void SortedVectorStructure(benchmark::State& state) {
    auto listOfNumbers = getRandomVector<DataType>(NumElements);
    std::sort(listOfNumbers.begin(), listOfNumbers.end());
    auto listOfCandidates = getShuffledData(listOfNumbers.begin(), listOfNumbers.end());
    for (auto _ : state) {
        for (auto candidate : listOfCandidates) {
            bool found = std::binary_search(listOfNumbers.begin(), listOfNumbers.end(), candidate);
            benchmark::DoNotOptimize(found);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(SortedVectorStructure);


static void SetStructure(benchmark::State& state) {
    auto listOfNumbers = getRandomSet<DataType>(NumElements);
    auto listOfCandidates = getShuffledData(listOfNumbers.begin(), listOfNumbers.end());
    for (auto _ : state) {
        for (auto candidate : listOfCandidates) {
            auto found = listOfNumbers.count(candidate) > 0;
            benchmark::DoNotOptimize(found);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(SetStructure);

static void UnorderedSetStructure(benchmark::State& state) {
    auto listOfNumbers = getRandomUnorderedSet<DataType>(NumElements);
    auto listOfCandidates = getShuffledData(listOfNumbers.begin(), listOfNumbers.end());
    for (auto _ : state) {
        for (auto candidate : listOfCandidates) {
            auto found = listOfNumbers.count(candidate) > 0;
            benchmark::DoNotOptimize(found);
        }
    }
}
// Register the function as a benchmark
BENCHMARK(UnorderedSetStructure);

BENCHMARK_MAIN();