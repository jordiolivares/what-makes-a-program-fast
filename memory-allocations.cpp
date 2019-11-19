#include <benchmark/benchmark.h>
#include <memory>

class DataElement {
public:
    DataElement(int key, int filler1, int filler2, int filler3, int filler4, int filler5, int filler6, int filler7)
            : key(key), filler1(filler1), filler2(filler2), filler3(filler3), filler4(filler4), filler5(filler5),
              filler6(filler6), filler7(filler7) {};

    int key;
    int filler1;
    int filler2;
    int filler3;
    int filler4;
    int filler5;
    int filler6;
    int filler7;
};
static constexpr auto NumElements = 2048;

static void HeapAllocation(benchmark::State &state) {
    for (auto _ : state) {
        std::vector<std::unique_ptr<DataElement>> array;
        array.reserve(NumElements);
        for (int i = 0; i < NumElements; i++) {
            array.emplace_back(std::make_unique<DataElement>(1, 1, 2, 3, 4, 5, 6, 7));
        }
        benchmark::DoNotOptimize(array);
    }
}
BENCHMARK(HeapAllocation);

static void StackAllocation(benchmark::State &state) {
    for (auto _ : state) {
        std::vector<DataElement> array;
        array.reserve(NumElements);
        for (int i = 0; i < NumElements; i++) {
            array.emplace_back(1, 1, 2, 3, 4, 5, 6, 7);
        }
        benchmark::DoNotOptimize(array);
    }
}
BENCHMARK(StackAllocation);

static void DataLocal(benchmark::State &state) {
    std::vector<DataElement> array;
    array.reserve(NumElements);
    for (int i = 0; i < NumElements; i++) {
        array.emplace_back(std::rand(), 1, 2, 3, 4, 5, 6, 7);
    }
    for (auto _ : state) {
        auto maxElement = std::max_element(array.begin(), array.end(), [](const auto &a, const auto &b) {
            return a.key < b.key;
        });
        benchmark::DoNotOptimize(maxElement);
    }
}
BENCHMARK(DataLocal);

static void DataIndirection(benchmark::State &state) {
    std::vector<std::unique_ptr<DataElement>> array;
    array.reserve(NumElements);
    for (int i = 0; i < NumElements; i++) {
        array.emplace_back(std::make_unique<DataElement>(std::rand(), 1, 2, 3, 4, 5, 6, 7));
    }
    for (auto _ : state) {
        auto maxElement = std::max_element(array.begin(), array.end(), [](const auto &a, const auto &b) {
            return a->key < b->key;
        });
        benchmark::DoNotOptimize(maxElement);
    }
}
BENCHMARK(DataIndirection);

BENCHMARK_MAIN();