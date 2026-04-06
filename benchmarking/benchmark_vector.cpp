#include "vector.hpp"
#include <benchmark/benchmark.h>
#include <benchmark/registration.h>
#include <benchmark/state.h>
#include <benchmark/utils.h>
#include <vector>

static void BM_CustomPushBack(benchmark::State &state) {
  for (auto _ : state) {
    Vector<int> v;
    v.reserve(state.range(0));
    for (auto i{0}; i < state.range(0); i++) {
      auto x{i};
      v.push_back(x);
    }
    benchmark::DoNotOptimize(v);
  }
}

static void BM_StandardPushBack(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> v;
    v.reserve(state.range(0));
    for (auto i{0}; i < state.range(0); i++) {
      v.push_back(i);
    }
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_CustomPushBack)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_StandardPushBack)->Range(1 << 10, 1 << 20);
BENCHMARK_MAIN();
