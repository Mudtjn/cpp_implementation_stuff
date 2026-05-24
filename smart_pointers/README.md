## Running Unit Tests

```bash
clang++ -g -std=c++23 unique_ptr_unit_test.cpp -lgtest -lgtest_main -pthread -o test && ./test
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