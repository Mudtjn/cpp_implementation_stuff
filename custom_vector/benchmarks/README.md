# Benchmarks

Compares custom `Vector<T>` against `std::vector` using [Google Benchmark](https://github.com/google/benchmark).

## Files

| File | Coverage |
|---|---|
| `benchmark_vector.cpp` | `push_back` (copy + move) with `std::string` |
| `benchmark_vector_ops.cpp` | constructors, assignment, modifiers, capacity, element access, iterators with `std::string` |

## Element Type

All benchmarks use `std::string(32, 'x')` — 32 chars, past SSO threshold (~15 chars on most impls).

**Why not `int`:** `int` is trivially copyable, no heap alloc, fits in cache — doesn't exercise the allocator at all. `std::string(32)` forces one heap allocation per element, making push_back, copy, and insert costs realistic.

## Build

Dependencies are fetched automatically via CMake FetchContent — no manual installs needed.

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release   # Release required for meaningful timings
make benchmarks                        # builds all benchmark executables
```

> **Always build Release.** Debug builds disable inlining — timings are 5-10x slower and not representative.

## Run

```bash
# from build/ directory
./benchmark_vector
./benchmark_vector_ops

# build + run via CMake custom target
make bench_benchmark_vector
make bench_benchmark_vector_ops
```

## Filter

```bash
./benchmark_vector_ops --benchmark_filter=Insert
./benchmark_vector_ops --benchmark_filter=BM_Custom   # custom only
./benchmark_vector_ops --benchmark_filter=BM_Std      # std only
```

## Output formats

```bash
./benchmark_vector_ops --benchmark_format=json > results.json
./benchmark_vector_ops --benchmark_format=csv  > results.csv
```

---

## Results (Release, 2026-04-26)

CPU: 16 × 2918.4 MHz | L1: 48 KiB | L2: 1280 KiB | L3: 24576 KiB  
Element type: `std::string(32, 'x')`

---

### push_back — `benchmark_vector`

| Size | Custom copy | Std copy | Custom move | Std move |
|---|---|---|---|---|
| 1024 | 55,815 ns | 48,591 ns | 51,117 ns | 47,107 ns |
| 4096 | 219,737 ns | 199,300 ns | 219,572 ns | 192,503 ns |
| 32768 | 1,801,441 ns | 1,650,361 ns | 1,716,608 ns | 1,563,265 ns |
| 262144 | 14,542,347 ns | 13,343,564 ns | 13,811,089 ns | 13,016,202 ns |
| 1048576 | 58,079,404 ns | 52,531,731 ns | 57,212,453 ns | 51,377,605 ns |

**Gap: ~10-15% slower than std.** Constant across all sizes — not algorithmic, just per-element overhead vs heavily optimized libstdc++.

**Copy vs Move gap is small (~8%)** because both paths allocate a 32-byte heap chunk per element:
- Copy: `new string(kStr)` = 1 heap alloc + 32-byte memcpy
- Move: `new string(std::string(32,'x'))` = create temp (1 heap alloc + fill) + pointer swap

Move advantage only appears when transferring an *already-allocated* string. Here both paths pay the same heap allocation cost.

---

### ops — `benchmark_vector_ops`

#### MoveCtor / MoveAssign

| Size | Custom | Std | Gap |
|---|---|---|---|
| 1024 | 53,818 ns | 49,254 ns | ~9% |
| 1048576 | 59,681,680 ns | 53,527,404 ns | ~11% |

**Gap: ~10% slower.** Both benchmarks include building source via `push_back` — that is where most time is spent. The actual move (pointer swap, O(1)) is noise. Gap is push_back overhead, not move overhead.

---

#### CopyAssign

| Size | Custom | Std | Gap |
|---|---|---|---|
| 1024 | 53,092 ns | 5,974 ns | **8.9x slower** |
| 4096 | 215,480 ns | 24,805 ns | **8.7x slower** |
| 262144 | 14,872,883 ns | 2,911,191 ns | **5.1x slower** |
| 1048576 | 62,103,484 ns | 10,972,798 ns | **5.7x slower** |

**Biggest real performance bug.** Root cause:

Our copy assign always allocates a new buffer regardless of existing capacity:
```cpp
arr = static_cast<T*>(::operator new(cap * sizeof(T)));  // malloc every call
std::uninitialized_copy(other.arr, other.arr + sz, arr);
```

`std::vector` reuses the existing buffer when `cap >= other.sz` — no malloc at all in the repeated benchmark iterations. We malloc + free every iteration.

**Fix**: check if existing capacity is sufficient before allocating.

---

#### PopBack / Clear

| Size | Custom | Std | Gap |
|---|---|---|---|
| 1024 (PopBack) | 52,786 ns | 49,129 ns | ~7% |
| 262144 (Clear) | 14,471,256 ns | 13,107,722 ns | ~10% |

Small constant gap — acceptable. Both destroy the same n string objects. Gap is implementation efficiency.

---

#### InsertSingle / InsertCount / InsertRange

| Size | Custom InsertSingle | Std InsertSingle | Gap |
|---|---|---|---|
| 1024 | 111,125 ns | 51,946 ns | **2.1x slower** |
| 4096 | 436,771 ns | 211,275 ns | **2.1x slower** |
| 262144 | 32,764,673 ns | 14,679,458 ns | **2.2x slower** |

**Root cause**: our shift loop uses copy-assignment:
```cpp
for (auto i = sz; i > index; i--)
    arr[i] = arr[i-1];   // string copy = heap alloc + memcpy per element
```

`std::vector` uses `std::move_backward` — string move = pointer swap, no heap alloc:
```cpp
std::move_backward(arr+index, arr+sz, arr+sz+1);   // O(n) pointer swaps only
```

Also: `arr[sz]` is raw uninitialized memory (`::operator new`). Assigning a string into it calls `operator=` which reads `this->_M_dataplus` (garbage) and may attempt to free a garbage pointer. **These insert benchmarks are technically UB** and results are unreliable for `std::string`. Fix required: use `std::uninitialized_move` for raw slots + `std::move_backward` for live slots.

---

#### Swap

| Size | Custom | Std |
|---|---|---|
| 1024–1048576 | ~3.1 ns | ~3.1 ns |

**Identical.** Both are O(1) pointer swap. ✓

---

#### Reserve

| Size | Custom | Std | Gap |
|---|---|---|---|
| 1024 | 1,297 ns | 59.3 ns | 21x |
| 262144 | 718,101 ns | 61.2 ns | 11,771x |
| 1048576 | 5,063,724 ns | 61.6 ns | 82,000x |

**Misleading result — not a real performance bug.** Std times (~60 ns, size-independent) indicate the compiler optimized the malloc away entirely: `DoNotOptimize(v)` forces the vector struct registers live but not the heap data, so the empty vector's allocation is elided.

Our custom destructor is less transparent to the optimizer → malloc+free actually executes. The scaling of our times is page-fault cost of touching fresh OS memory pages. A real-world reserve (followed by push_backs) shows comparable performance — see `ShrinkToFit` which does real work and shows only a 2.5x gap.

---

#### ShrinkToFit

| Size | Custom | Std | Gap |
|---|---|---|---|
| 1024 | 162,526 ns | 56,819 ns | 2.9x |
| 1048576 | 211,564,908 ns | 83,255,968 ns | 2.5x |

Both reallocate and move all n strings — should be similar cost. The ~2.5x gap suggests our `uninitialized_move + destroy` path has more overhead per element than std's fused move. Likely a missed optimization in the double-loop (move all, then destroy all) vs std's single pass.

---

#### At / operator[] / Iterators

| Benchmark | Gap |
|---|---|
| `at()` | ~0% (within noise) |
| `operator[]` | ~0% |
| ForwardIterate | ~0% |
| ReverseIterate | ~2% |

**Identical.** Same data layout, same access pattern. ✓

---

## Summary

| Operation | Gap | Status |
|---|---|---|
| push_back | ~10% | Expected — libstdc++ is years of tuning |
| MoveCtor/Assign | ~10% | Same — dominated by push_back build cost |
| PopBack / Clear | ~7-10% | Minor, acceptable |
| CopyAssign | **5-8x** | **Bug** — always mallocs; should reuse buffer |
| InsertSingle/Count/Range | **~2x** | **Bug** — copy-assign into raw slots (UB) + no move_backward |
| ShrinkToFit | 2.5x | Over-complex move path |
| Swap, access, iterate | ~0% | Correct ✓ |
| Reserve | 80,000x | Misleading — compiler optimizes std away |

## Known Omitted Benchmarks

`SizeValueCtor` and `CopyCtor` benchmarks are omitted — those constructors use `operator new` (raw memory) + `std::fill`/`std::copy` which is UB for `std::string`. Re-enable after completing `operator new` transition in `vector.hpp` (replace all `std::fill`/`std::copy` on raw memory with `std::uninitialized_fill`/`std::uninitialized_copy`).

## Rebuild after source changes

```bash
cd build
make benchmark_vector_ops   # cmake not needed unless CMakeLists.txt changed
```
