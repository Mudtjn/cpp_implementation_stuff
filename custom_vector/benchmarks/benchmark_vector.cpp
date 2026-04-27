#include "../vector.hpp"
#include <benchmark/benchmark.h>
#include <string>
#include <vector>

// 32 chars — past SSO threshold (~15) on most impls, forces heap alloc per element
static const std::string kStr(32, 'x');

static void BM_CustomPushBack(benchmark::State &state) {
  for (auto _ : state) {
    Vector<std::string> v;
    v.reserve(state.range(0));
    for (auto i{0}; i < state.range(0); i++)
      v.push_back(kStr);
    benchmark::DoNotOptimize(v);
  }
}

static void BM_StandardPushBack(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<std::string> v;
    v.reserve(state.range(0));
    for (auto i{0}; i < state.range(0); i++)
      v.push_back(kStr);
    benchmark::DoNotOptimize(v);
  }
}

static void BM_CustomPushBackMove(benchmark::State &state) {
  for (auto _ : state) {
    Vector<std::string> v;
    v.reserve(state.range(0));
    for (auto i{0}; i < state.range(0); i++)
      v.push_back(std::string(32, 'x'));  // rvalue — move overload, no copy
    benchmark::DoNotOptimize(v);
  }
}

static void BM_StandardPushBackMove(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<std::string> v;
    v.reserve(state.range(0));
    for (auto i{0}; i < state.range(0); i++)
      v.push_back(std::string(32, 'x'));
    benchmark::DoNotOptimize(v);
  }
}

BENCHMARK(BM_CustomPushBack)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_StandardPushBack)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_CustomPushBackMove)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_StandardPushBackMove)->Range(1 << 10, 1 << 20);
BENCHMARK_MAIN();
