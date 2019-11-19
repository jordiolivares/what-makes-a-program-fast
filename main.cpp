#include <benchmark/benchmark.h>

#include "ColumnarDatabase.hpp"

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



BENCHMARK_MAIN();