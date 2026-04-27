## Running Unit Tests

```bash
clang++ -g -std=c++23 vector_unit_test.cpp -lgtest -lgtest_main -pthread -o test && ./test
```

Requirements: `libgtest-dev` installed (`sudo apt install libgtest-dev`).

For memory/UB checks:
```bash
clang++ -g -std=c++23 -fsanitize=address,undefined vector_unit_test.cpp -lgtest -lgtest_main -pthread -o test_asan && ./test_asan
```

Alternatively via CMake (used by CI):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

---

## Benchmarks

Current implementation:

```
BENCHMARKS ON PUSH_BACK

2026-04-06T20:52:19+00:00
Running ./benchmark_vector
Run on (16 X 2918.4 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 1280 KiB (x8)
  L3 Unified 24576 KiB (x1)
Load Average: 0.37, 0.69, 1.10
----------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
BM_CustomPushBack/1024             774 ns          774 ns       910530
BM_CustomPushBack/4096            2826 ns         2826 ns       246778
BM_CustomPushBack/32768          22308 ns        22307 ns        31747
BM_CustomPushBack/262144        179020 ns       179021 ns         3920
BM_CustomPushBack/1048576       751539 ns       751507 ns          917
BM_StandardPushBack/1024           934 ns          934 ns       750268
BM_StandardPushBack/4096          3661 ns         3661 ns       188039
BM_StandardPushBack/32768        37442 ns        37439 ns        19622
BM_StandardPushBack/262144      280919 ns       280916 ns         2217
BM_StandardPushBack/1048576    1130220 ns      1130200 ns          575
```
