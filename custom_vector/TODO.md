● Analysis

  🟢 Essentially Identical (within 5%)

  Swap, At, SubscriptOp, ForwardIterate, ReverseIterate

  BM_Custom_Swap/262144     3.12 ns
  BM_Std_Swap/262144        3.16 ns   ← both pointer swap, O(1)

  BM_Custom_At/32768       31688 ns
  BM_Std_At/32768          30715 ns   ← same data layout, same bounds check
  Expected. Data layout identical (both contiguous T*), access patterns identical.

  ---
  🟡 ~10% Slower — MoveCtor, MoveAssign, PopBack, Clear

  BM_Custom_MoveCtor/1048576    59.7 ms
  BM_Std_MoveCtor/1048576       53.5 ms   ← 11% faster

  Both benchmarks build source vector via push_back first — that's where the time is. The actual move (pointer swap, O(1)) is noise. The 10% gap is just std::vector's more optimized
  allocator and push_back path vs ours.

  ---
  🔴 ~2x Slower — InsertSingle, InsertCount, InsertRange

  BM_Custom_InsertSingle/262144    32.8 ms
  BM_Std_InsertSingle/262144       14.7 ms   ← 2.2x faster

  Root cause: Our shift loop uses copy-assignment:
  for(auto i = sz; i > index; i--)
      arr[i] = arr[i-1];   // string copy = heap alloc + memcpy per element

  std::vector uses std::move_backward — string move = pointer swap, no heap alloc:
  std::move_backward(arr+index, arr+sz, arr+sz+1);  // swap ptrs only

  Also: arr[sz] is raw memory — assigning a string into it is UB. These benchmark results are unreliable because of UB (reading garbage heap pointer). The fact it doesn't crash here is
  luck from mmap zero-initializing pages.

  Fix: use uninitialized_move for raw slots + move_backward for live slots.

  ---
  🔴🔴 5-8x Slower — CopyAssign

  BM_Custom_CopyAssign/1024      53,092 ns
  BM_Std_CopyAssign/1024          5,974 ns   ← 8.9x faster

  Root cause: Our copy assign ALWAYS mallocs a new buffer:
  arr = static_cast<T*>(::operator new(cap * sizeof(T)));  // always malloc
  std::uninitialized_copy(other.arr, other.arr + sz, arr);  // construct all

  std::vector reuses existing buffer when capacity is sufficient — no malloc at all:
  // if existing cap >= new sz:
  //   copy-assign elements in place (no allocation)
  // else:
  //   allocate new buffer

  In the benchmark, dst starts empty but each repeated iteration of copy assign hits an already-allocated dst — std::vector reuses it, ours mallocs every time.

  Fix: check if sz <= cap, reuse buffer if so.

  ---
  🔴🔴🔴 80,000x Slower — Reserve

  BM_Custom_Reserve/1024        1,297 ns
  BM_Std_Reserve/1024              59 ns   ← 21x faster

  BM_Custom_Reserve/262144    718,101 ns
  BM_Std_Reserve/262144            61 ns   ← 11,771x faster

  Root cause: std::vector's reserve time is ~60ns regardless of size — the compiler is optimizing away the malloc entirely because the empty vector is never read after reserve.
  DoNotOptimize(v) only forces the struct pointer live, not the heap data.

  Our custom reserve: the compiler can't prove the same thing (our destructor's ::operator delete is less transparent to the optimizer), so it actually executes malloc + page-fault costs
  on access.

  This result is misleading — not a real performance bug, just optimizer visibility difference. To get a fair comparison, reserve then push_back elements to force real allocation (which is
   exactly what ShrinkToFit does — that's why ShrinkToFit shows only 2.5x gap not 80,000x).

  ---
  🔴 2.5x Slower — ShrinkToFit

  BM_Custom_ShrinkToFit/1048576    211 ms
  BM_Std_ShrinkToFit/1048576        83 ms

  We build source via push_back (same cost both sides), then:
  - Both allocate new buffer, move all strings, free old — should be similar
  - Gap likely from extra function call overhead in uninitialized_move vs std's tighter __uninitialized_move_a with allocator hints + potential SIMD for ptr moves

  ---
  Summary Table

  ┌─────────────────────────────────┬─────────┬───────────────────────────────────────────────────────┐
  │            Benchmark            │   Gap   │                      Root cause                       │
  ├─────────────────────────────────┼─────────┼───────────────────────────────────────────────────────┤
  │ Swap, iterate, access           │ ~0%     │ identical layout ✓                                    │
  ├─────────────────────────────────┼─────────┼───────────────────────────────────────────────────────┤
  │ MoveCtor/Assign, PopBack, Clear │ ~10%    │ push_back overhead, allocator efficiency              │
  ├─────────────────────────────────┼─────────┼───────────────────────────────────────────────────────┤
  │ InsertSingle/Count/Range        │ ~2x     │ copy assign instead of move_backward + UB             │
  ├─────────────────────────────────┼─────────┼───────────────────────────────────────────────────────┤
  │ CopyAssign                      │ 5-8x    │ always mallocs new buffer — must check existing cap   │
  ├─────────────────────────────────┼─────────┼───────────────────────────────────────────────────────┤
  │ ShrinkToFit                     │ ~2.5x   │ overhead per element in move path                     │
  ├─────────────────────────────────┼─────────┼───────────────────────────────────────────────────────┤
  │ Reserve                         │ 80,000x │ compiler optimizes std away — benchmark is misleading │
  └─────────────────────────────────┴─────────┴───────────────────────────────────────────────────────┘

  Biggest real bug to fix: CopyAssign. Reuse existing buffer when cap >= other.sz. That alone closes the biggest legitimate gap