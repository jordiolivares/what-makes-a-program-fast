#include <benchmark/benchmark.h>
#include <Eigen/Eigen>

static constexpr auto NumColumns = 2048;
static constexpr auto NumRows = 2048;

static void MatchingLayout(benchmark::State &state) {
    auto matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>::Random(NumRows, NumColumns);
    for (auto _ : state) {
        auto result = matrix.colwise().maxCoeff().sum();
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(MatchingLayout);

static void NonMatchingLayout(benchmark::State &state) {
    auto matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>::Random(NumRows, NumColumns);
    for (auto _ : state) {
        auto result = matrix.colwise().maxCoeff().sum();
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(NonMatchingLayout);

BENCHMARK_MAIN();