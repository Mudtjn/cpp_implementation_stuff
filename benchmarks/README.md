# Benchmarks

Compares custom `Vector<T>` against `std::vector` using [Google Benchmark](https://github.com/google/benchmark).

## Files

| File | Coverage |
|---|---|
| `benchmark_vector.cpp` | `push_back` |
| `benchmark_vector_ops.cpp` | constructors, assignment, modifiers, capacity, element access, iterators |

## Build

Dependencies are fetched automatically via CMake FetchContent — no manual installs needed.

```bash
# first time or after CMakeLists.txt changes
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release   # Release required for meaningful timings
make benchmarks                        # builds all benchmark executables
```

> **Always build Release.** Debug builds disable inlining and add iterator
> validation — timings are 5-10x slower and not representative.

## Run on Release Build

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make benchmark_vector_ops
./benchmark_vector_ops
```

Or build + run all in one step:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release && make benchmarks
./benchmark_vector
./benchmark_vector_ops
```

## Run

```bash
# from build/ directory

# run specific benchmark
./benchmark_vector
./benchmark_vector_ops

# build + run in one step (CMake custom target)
make bench_benchmark_vector
make bench_benchmark_vector_ops
```

## Filter by benchmark name

```bash
./benchmark_vector_ops --benchmark_filter=PushBack
./benchmark_vector_ops --benchmark_filter=Insert
./benchmark_vector_ops --benchmark_filter=BM_Custom   # custom only
./benchmark_vector_ops --benchmark_filter=BM_Std      # std only
```

## Output format options

```bash
./benchmark_vector_ops --benchmark_format=json > results.json
./benchmark_vector_ops --benchmark_format=csv  > results.csv
```

## Running Unit Tests on Release Build

Tests default to Debug. To verify correctness with Release optimizations:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make vector_unit_tests
./vector_unit_tests
```

Or use the CMake `check` target:

```bash
make check   # builds vector_unit_tests then runs it
```

> Run tests on Release before trusting benchmark numbers — compiler optimizations
> can expose UB that Debug masks (e.g. signed overflow, dangling pointers).
> If tests pass Debug but fail Release, there is undefined behavior in the code.

## Rebuild after source changes

```bash
cd build
make benchmark_vector_ops   # cmake not needed — just make
```

`cmake ..` only needed again when `CMakeLists.txt` changes or new benchmark
files are added to this folder.
