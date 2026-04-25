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

## Results Summary (Release build, 2026-04-24)

CPU: 16 x 2918.4 MHz | L1: 48 KiB | L2: 1280 KiB | L3: 24576 KiB

### push_back (benchmark_vector)

| Size | Custom | std::vector | Winner |
|---|---|---|---|
| 1024 | 937 ns | 854 ns | std ~10% |
| 4096 | 3370 ns | 3476 ns | custom ~3% |
| 32768 | 33678 ns | 33442 ns | std ~0.7% |
| 1048576 | 1142776 ns | 1247252 ns | custom ~9% |

Essentially identical — within noise margin.

### Key findings (benchmark_vector_ops)

| Operation | Custom | std::vector | Notes |
|---|---|---|---|
| `SizeCtor` | ~45 ns (flat) | scales linearly | compiler eliminates zero-init + demand paging |
| `SizeValueCtor` | identical | identical | both use fill |
| `CopyCtor` | identical | identical | after `std::copy` fix |
| `CopyAssign` | 102 ns (1K) | 63 ns (1K) | std reuses capacity, custom always allocates |
| `MoveCtor` | identical | identical | both O(1) pointer swap |
| `MoveAssign` | identical | identical | both O(1) pointer swap |
| `PopBack` | identical | identical | |
| `Clear` | identical | identical | |
| `InsertSingle` | ~10% slower | — | scalar shift vs memmove |
| `InsertCount` | ~10% slower | — | same reason |
| `InsertRange` | ~14% slower | — | same reason |
| `ResizeWithValue` | identical | identical | after `std::fill` fix (was 6x slower) |
| `Swap` | ~3.7 ns | ~3.0 ns | both O(1), std slightly faster |
| `Reserve` | identical | identical | |
| `ShrinkToFit` | identical | identical | |
| `at()` | slightly slower | — | extra function call overhead |
| `operator[]` | identical | identical | after removing bounds check (was 3.3x slower) |
| `ForwardIterate` | identical | identical | |
| `ReverseIterate` | identical | identical | |

### Optimizations applied during benchmarking

| Bug found | Fix | Impact |
|---|---|---|
| `operator[]` had bounds check | removed — direct `arr[i]` | 3.3x speedup |
| Copy ctor/assign used for-loop | replaced with `std::copy` | 5-10x speedup |
| `resize(n, val)` used `push_back` loop | replaced with `std::fill` | 6x speedup |
| Insert used scalar shift loop | replaced with `std::memmove` | ~10% speedup |

### Known remaining gaps

| Gap | Root cause |
|---|---|
| `SizeCtor` flat timing | compiler eliminates `new T[n]` zero-init + Linux demand paging |
| `CopyAssign` small-size gap | custom always allocates; std reuses existing capacity |
| `InsertSingle/Count/Range` ~10% slower | `memmove` applied but std may use additional tricks |

## Rebuild after source changes

```bash
cd build
make benchmark_vector_ops   # cmake not needed — just make
```

`cmake ..` only needed again when `CMakeLists.txt` changes or new benchmark
files are added to this folder.
