#include "../vector.hpp"
#include <benchmark/benchmark.h>
#include <string>
#include <vector>

// 32 chars — forces heap alloc per element (past SSO ~15 char threshold)
static const std::string kStr(32, 'x');

static Vector<std::string> make_custom(int n) {
  Vector<std::string> v;
  v.reserve(n);
  for (int i = 0; i < n; i++) v.push_back(kStr);
  return v;
}

static std::vector<std::string> make_std(int n) {
  std::vector<std::string> v;
  v.reserve(n);
  for (int i = 0; i < n; i++) v.push_back(kStr);
  return v;
}

// ─── constructors ───────────────────────────────────────────────────────────

// NOTE: SizeValueCtor and CopyCtor benchmarks omitted —
// Vector(size, value) uses operator new + std::fill on raw memory (UB for string).
// Copy ctor uses operator new + std::copy on raw memory (UB for string).
// Re-enable after completing operator new transition in vector.hpp.

static void BM_Custom_MoveCtor(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_custom(state.range(0));
    Vector<std::string> v(std::move(src));
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_MoveCtor(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_std(state.range(0));
    std::vector<std::string> v(std::move(src));
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_MoveCtor)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_MoveCtor)->Range(1 << 10, 1 << 20);

// ─── assignment ─────────────────────────────────────────────────────────────

static void BM_Custom_MoveAssign(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_custom(state.range(0));
    Vector<std::string> dst;
    dst = std::move(src);
    benchmark::DoNotOptimize(dst);
  }
}
static void BM_Std_MoveAssign(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_std(state.range(0));
    std::vector<std::string> dst;
    dst = std::move(src);
    benchmark::DoNotOptimize(dst);
  }
}
BENCHMARK(BM_Custom_MoveAssign)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_MoveAssign)->Range(1 << 10, 1 << 20);

// copy assign uses new T[] (old path) — safe for string
static void BM_Custom_CopyAssign(benchmark::State &state) {
  auto src = make_custom(state.range(0));
  Vector<std::string> dst;
  for (auto _ : state) {
    dst = src;
    benchmark::DoNotOptimize(dst);
  }
}
static void BM_Std_CopyAssign(benchmark::State &state) {
  auto src = make_std(state.range(0));
  std::vector<std::string> dst;
  for (auto _ : state) {
    dst = src;
    benchmark::DoNotOptimize(dst);
  }
}
BENCHMARK(BM_Custom_CopyAssign)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_CopyAssign)->Range(1 << 10, 1 << 20);

// ─── modifiers ──────────────────────────────────────────────────────────────

static void BM_Custom_PopBack(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    while (!v.empty()) v.pop_back();
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_PopBack(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    while (!v.empty()) v.pop_back();
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_PopBack)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_PopBack)->Range(1 << 10, 1 << 18);

static void BM_Custom_Clear(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.clear();
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_Clear(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.clear();
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_Clear)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_Clear)->Range(1 << 10, 1 << 18);

static void BM_Custom_InsertSingle(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.insert(v.begin() + v.size() / 2, kStr);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_InsertSingle(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.insert(v.begin() + v.size() / 2, kStr);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_InsertSingle)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_InsertSingle)->Range(1 << 10, 1 << 18);

static void BM_Custom_InsertCount(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.insert(v.begin(), 100, kStr);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_InsertCount(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.insert(v.begin(), 100, kStr);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_InsertCount)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_InsertCount)->Range(1 << 10, 1 << 18);

static void BM_Custom_InsertRange(benchmark::State &state) {
  std::vector<std::string> src(100, kStr);
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.insert(v.begin(), src.begin(), src.end());
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_InsertRange(benchmark::State &state) {
  std::vector<std::string> src(100, kStr);
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.insert(v.begin(), src.begin(), src.end());
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_InsertRange)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_InsertRange)->Range(1 << 10, 1 << 18);

static void BM_Custom_Swap(benchmark::State &state) {
  auto v1 = make_custom(state.range(0));
  auto v2 = make_custom(state.range(0) / 2);
  for (auto _ : state) {
    v1.swap(v2);
    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);
  }
}
static void BM_Std_Swap(benchmark::State &state) {
  auto v1 = make_std(state.range(0));
  auto v2 = make_std(state.range(0) / 2);
  for (auto _ : state) {
    v1.swap(v2);
    benchmark::DoNotOptimize(v1);
    benchmark::DoNotOptimize(v2);
  }
}
BENCHMARK(BM_Custom_Swap)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_Swap)->Range(1 << 10, 1 << 20);

// ─── capacity ───────────────────────────────────────────────────────────────

static void BM_Custom_Reserve(benchmark::State &state) {
  for (auto _ : state) {
    Vector<std::string> v;
    v.reserve(state.range(0));
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_Reserve(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<std::string> v;
    v.reserve(state.range(0));
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_Reserve)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_Reserve)->Range(1 << 10, 1 << 20);

static void BM_Custom_ShrinkToFit(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.reserve(state.range(0) * 2);
    v.shrink_to_fit();
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_ShrinkToFit(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.reserve(state.range(0) * 2);
    v.shrink_to_fit();
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_ShrinkToFit)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_ShrinkToFit)->Range(1 << 10, 1 << 20);

// ─── element access ─────────────────────────────────────────────────────────

static void BM_Custom_At(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (int i = 0; i < state.range(0); i++)
      len += v.at(i).size();
    benchmark::DoNotOptimize(len);
  }
}
static void BM_Std_At(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (int i = 0; i < state.range(0); i++)
      len += v.at(i).size();
    benchmark::DoNotOptimize(len);
  }
}
BENCHMARK(BM_Custom_At)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_At)->Range(1 << 10, 1 << 18);

static void BM_Custom_SubscriptOp(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (int i = 0; i < state.range(0); i++)
      len += v[i].size();
    benchmark::DoNotOptimize(len);
  }
}
static void BM_Std_SubscriptOp(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (int i = 0; i < state.range(0); i++)
      len += v[i].size();
    benchmark::DoNotOptimize(len);
  }
}
BENCHMARK(BM_Custom_SubscriptOp)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_SubscriptOp)->Range(1 << 10, 1 << 18);

// ─── iterators ──────────────────────────────────────────────────────────────

static void BM_Custom_ForwardIterate(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
      len += it->size();
    benchmark::DoNotOptimize(len);
  }
}
static void BM_Std_ForwardIterate(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
      len += it->size();
    benchmark::DoNotOptimize(len);
  }
}
BENCHMARK(BM_Custom_ForwardIterate)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_ForwardIterate)->Range(1 << 10, 1 << 18);

static void BM_Custom_ReverseIterate(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
      len += it->size();
    benchmark::DoNotOptimize(len);
  }
}
static void BM_Std_ReverseIterate(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    std::size_t len = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
      len += it->size();
    benchmark::DoNotOptimize(len);
  }
}
BENCHMARK(BM_Custom_ReverseIterate)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_ReverseIterate)->Range(1 << 10, 1 << 18);

BENCHMARK_MAIN();
