#include "../vector.hpp"
#include <benchmark/benchmark.h>
#include <numeric>
#include <vector>

// ─── helpers ────────────────────────────────────────────────────────────────

static Vector<int> make_custom(int n) {
  Vector<int> v;
  v.reserve(n);
  for (int i = 0; i < n; i++) v.push_back(i);
  return v;
}

static std::vector<int> make_std(int n) {
  std::vector<int> v;
  v.reserve(n);
  for (int i = 0; i < n; i++) v.push_back(i);
  return v;
}

// ─── constructors ───────────────────────────────────────────────────────────

static void BM_Custom_SizeCtor(benchmark::State &state) {
  for (auto _ : state) {
    Vector<int> v(state.range(0));
    benchmark::DoNotOptimize(v.data()); // make heap pointer observable — prevents allocation elimination
    benchmark::ClobberMemory();         // force page faults: compiler must assume all memory changed,
                                        // cannot eliminate zero-init writes → timing scales with n
  }
}
static void BM_Std_SizeCtor(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> v(state.range(0));
    benchmark::DoNotOptimize(v.data());
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_Custom_SizeCtor)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_SizeCtor)->Range(1 << 10, 1 << 20);

static void BM_Custom_SizeValueCtor(benchmark::State &state) {
  for (auto _ : state) {
    Vector<int> v(state.range(0), 42);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_SizeValueCtor(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> v(state.range(0), 42);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_SizeValueCtor)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_SizeValueCtor)->Range(1 << 10, 1 << 20);

static void BM_Custom_CopyCtor(benchmark::State &state) {
  auto src = make_custom(state.range(0));
  for (auto _ : state) {
    Vector<int> v(src);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_CopyCtor(benchmark::State &state) {
  auto src = make_std(state.range(0));
  for (auto _ : state) {
    std::vector<int> v(src);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_CopyCtor)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_CopyCtor)->Range(1 << 10, 1 << 20);

static void BM_Custom_MoveCtor(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_custom(state.range(0));
    Vector<int> v(std::move(src));
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_MoveCtor(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_std(state.range(0));
    std::vector<int> v(std::move(src));
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_MoveCtor)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_MoveCtor)->Range(1 << 10, 1 << 20);

// ─── assignment ─────────────────────────────────────────────────────────────

static void BM_Custom_CopyAssign(benchmark::State &state) {
  auto src = make_custom(state.range(0));
  Vector<int> dst;
  for (auto _ : state) {
    dst = src;
    benchmark::DoNotOptimize(dst);
  }
}
static void BM_Std_CopyAssign(benchmark::State &state) {
  auto src = make_std(state.range(0));
  std::vector<int> dst;
  for (auto _ : state) {
    dst = src;
    benchmark::DoNotOptimize(dst);
  }
}
BENCHMARK(BM_Custom_CopyAssign)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_CopyAssign)->Range(1 << 10, 1 << 20);

static void BM_Custom_MoveAssign(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_custom(state.range(0));
    Vector<int> dst;
    dst = std::move(src);
    benchmark::DoNotOptimize(dst);
  }
}
static void BM_Std_MoveAssign(benchmark::State &state) {
  for (auto _ : state) {
    auto src = make_std(state.range(0));
    std::vector<int> dst;
    dst = std::move(src);
    benchmark::DoNotOptimize(dst);
  }
}
BENCHMARK(BM_Custom_MoveAssign)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_MoveAssign)->Range(1 << 10, 1 << 20);

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
BENCHMARK(BM_Custom_PopBack)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_PopBack)->Range(1 << 10, 1 << 20);

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
BENCHMARK(BM_Custom_Clear)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_Clear)->Range(1 << 10, 1 << 20);

static void BM_Custom_InsertSingle(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.insert(v.begin() + v.size() / 2, 99);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_InsertSingle(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.insert(v.begin() + v.size() / 2, 99);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_InsertSingle)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_InsertSingle)->Range(1 << 10, 1 << 18);

static void BM_Custom_InsertCount(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.insert(v.begin(), 100, 42);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_InsertCount(benchmark::State &state) {
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.insert(v.begin(), 100, 42);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_InsertCount)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_InsertCount)->Range(1 << 10, 1 << 18);

static void BM_Custom_InsertRange(benchmark::State &state) {
  std::vector<int> src(100);
  std::iota(src.begin(), src.end(), 0);
  for (auto _ : state) {
    auto v = make_custom(state.range(0));
    v.insert(v.begin(), src.begin(), src.end());
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_InsertRange(benchmark::State &state) {
  std::vector<int> src(100);
  std::iota(src.begin(), src.end(), 0);
  for (auto _ : state) {
    auto v = make_std(state.range(0));
    v.insert(v.begin(), src.begin(), src.end());
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_InsertRange)->Range(1 << 10, 1 << 18);
BENCHMARK(BM_Std_InsertRange)->Range(1 << 10, 1 << 18);

static void BM_Custom_Resize(benchmark::State &state) {
  for (auto _ : state) {
    Vector<int> v;
    v.resize(state.range(0));
    benchmark::DoNotOptimize(v.data()); // prevent allocation elimination
    benchmark::ClobberMemory();         // force zero-init writes to actually happen
  }
}
static void BM_Std_Resize(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> v;
    v.resize(state.range(0));
    benchmark::DoNotOptimize(v.data());
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_Custom_Resize)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_Resize)->Range(1 << 10, 1 << 20);

static void BM_Custom_ResizeWithValue(benchmark::State &state) {
  for (auto _ : state) {
    Vector<int> v;
    v.resize(state.range(0), 42);
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_ResizeWithValue(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> v;
    v.resize(state.range(0), 42);
    benchmark::DoNotOptimize(v);
  }
}
BENCHMARK(BM_Custom_ResizeWithValue)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_ResizeWithValue)->Range(1 << 10, 1 << 20);

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
    Vector<int> v;
    v.reserve(state.range(0));
    benchmark::DoNotOptimize(v);
  }
}
static void BM_Std_Reserve(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> v;
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
    int sum = 0;
    for (int i = 0; i < state.range(0); i++)
      sum += v.at(i);
    benchmark::DoNotOptimize(sum);
  }
}
static void BM_Std_At(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < state.range(0); i++)
      sum += v.at(i);
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_Custom_At)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_At)->Range(1 << 10, 1 << 20);

static void BM_Custom_SubscriptOp(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < state.range(0); i++)
      sum += v[i];
    benchmark::DoNotOptimize(sum);
  }
}
static void BM_Std_SubscriptOp(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < state.range(0); i++)
      sum += v[i];
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_Custom_SubscriptOp)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_SubscriptOp)->Range(1 << 10, 1 << 20);

// ─── iterators ──────────────────────────────────────────────────────────────

static void BM_Custom_ForwardIterate(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
      sum += *it;
    benchmark::DoNotOptimize(sum);
  }
}
static void BM_Std_ForwardIterate(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (auto it = v.begin(); it != v.end(); ++it)
      sum += *it;
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_Custom_ForwardIterate)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_ForwardIterate)->Range(1 << 10, 1 << 20);

static void BM_Custom_ReverseIterate(benchmark::State &state) {
  auto v = make_custom(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
      sum += *it;
    benchmark::DoNotOptimize(sum);
  }
}
static void BM_Std_ReverseIterate(benchmark::State &state) {
  auto v = make_std(state.range(0));
  for (auto _ : state) {
    int sum = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it)
      sum += *it;
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(BM_Custom_ReverseIterate)->Range(1 << 10, 1 << 20);
BENCHMARK(BM_Std_ReverseIterate)->Range(1 << 10, 1 << 20);

BENCHMARK_MAIN();
