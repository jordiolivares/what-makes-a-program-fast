#include <benchmark/benchmark.h>
#include <memory>
#include <random>

#include <xsimd/xsimd.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xmath.hpp>

#include "ColumnarDatabase.hpp"

class DataElement {
public:
    DataElement(float key, int filler1, int filler2, int filler3, int filler4, int filler5, int filler6, int filler7)
            : key(key), filler1(filler1), filler2(filler2), filler3(filler3), filler4(filler4), filler5(filler5),
              filler6(filler6), filler7(filler7) {};

    float key;
    int filler1;
    int filler2;
    int filler3;
    int filler4;
    int filler5;
    int filler6;
    int filler7;
};

static constexpr auto NumElements = 65536;

std::vector<std::unique_ptr<DataElement>> makeDataElements(int numElements) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0);
    std::vector<std::unique_ptr<DataElement>> dataElements;
    for (int i = 0; i < numElements; i++) {
        dataElements.emplace_back(std::make_unique<DataElement>(dis(gen), 1, 2, 3, 4, 5, 6, 7));
    }
    return dataElements;
}

std::vector<DataElement> makeDataElementsLocal(int numElements) {
    auto elements = makeDataElements(numElements);
    std::vector<DataElement> localElements;
    for (auto &element : elements) {
        localElements.emplace_back(element->key, element->filler1, element->filler2, element->filler3, element->filler4, element->filler5, element->filler6, element->filler7);
    }
    return localElements;
}

std::vector<DataElement> makeDataElementsLocalSorted(int numElements) {
    auto elements = makeDataElementsLocal(numElements);
    std::sort(elements.begin(), elements.end(), [](const auto &a, const auto &b) {
        return a.key < b.key;
    });
    return elements;
}

Database<float, int, int, int, int, int, int, int> makeDataElementsColumnar(int numElements) {
    auto elements = makeDataElementsLocalSorted(numElements);
    Database<float, int, int, int, int, int, int, int> database;
    for (auto &element : elements) {
        database.emplace(element.key, element.filler1, element.filler2, element.filler3, element.filler4, element.filler5, element.filler6, element.filler7);
    }
    return database;
}

static void naiveVersion(benchmark::State &state) {
    auto elements = makeDataElements(NumElements);
    for (auto _ : state) {
        float total = 0;
        for (auto &element : elements) {
            if (element->key < 0.5) {
                total += element->key;
            }
        }
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(naiveVersion);

static void localVersion(benchmark::State &state) {
    auto elements = makeDataElementsLocal(NumElements);
    for (auto _ : state) {
        float total = 0;
        for (auto &element : elements) {
            if (element.key < 0.5) {
                total += element.key;
            }
        }
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(localVersion);

static void localSortedVersion(benchmark::State &state) {
    auto elements = makeDataElementsLocalSorted(NumElements);
    for (auto _ : state) {
        float total = 0;
        for (auto &element : elements) {
            if (element.key < 0.5) {
                total += element.key;
            }
        }
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(localSortedVersion);

static void columnarSortedVersion(benchmark::State &state) {
    auto elements = makeDataElementsColumnar(NumElements);
    auto &column = elements.getColumn<0>();
    for (auto _ : state) {
        auto end = std::find_if(column.begin(), column.end(), [](const auto &a) {
            return a >= 0.5;
        });
        float total = std::accumulate(column.begin(), end, 0.f);
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(columnarSortedVersion);

float sum(const float *begin, const float *end) {
    constexpr auto simd_size = xsimd::simd_type<float>::size;
    xsimd::batch<float, simd_size> result(0.f);
    auto numElements = std::distance(begin, end);
    auto vec_size = numElements - numElements % simd_size;
    for (int i = 0; i < vec_size; i += simd_size) {
        auto batch = xsimd::load_unaligned(begin + i);
        result += batch;
    }
    return std::accumulate(result.begin(), result.end(), 0.f) + std::accumulate(begin + vec_size, end, 0.f);
}

static void columnarLibrarySimdSortedVersion(benchmark::State &state) {
    auto elements = makeDataElementsColumnar(NumElements);
    auto &column = elements.getColumn<0>();
    for (auto _ : state) {
        auto end = std::find_if(column.begin(), column.end(), [](const auto &a) {
            return a >= 0.5;
        });
        auto numElements = std::distance(column.begin(), end);
        std::array<std::size_t, 1> shape = {static_cast<unsigned long>(numElements)};
        auto tensor = xt::adapt(column.data(), shape[0], xt::no_ownership(), shape);
        float total = xt::sum(tensor)();
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(columnarLibrarySimdSortedVersion);

static void columnarSimdSortedVersion(benchmark::State &state) {
    auto elements = makeDataElementsColumnar(NumElements);
    auto &column = elements.getColumn<0>();
    for (auto _ : state) {
        auto end = std::find_if(column.begin(), column.end(), [](const auto &a) {
            return a >= 0.5;
        });
        float total = sum(column.data(), end.base());
        benchmark::DoNotOptimize(total);
    }
}
BENCHMARK(columnarSimdSortedVersion);

BENCHMARK_MAIN();