cmake_minimum_required(VERSION 3.20)
project(VectorProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
enable_testing()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_Declare(
  googlebenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.zip
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest googlebenchmark)

add_library(vector_lib vector.cpp)
target_include_directories(vector_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(vector_unit_tests vector_unit_test.cpp)
target_link_libraries(vector_unit_tests PRIVATE vector_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(vector_unit_tests)

add_custom_target(check
  COMMAND vector_unit_tests
  DEPENDS vector_unit_tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running unit tests"
)

file(GLOB BENCHMARK_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/*.cpp"
)

add_custom_target(benchmarks
  COMMENT "Build all benchmark executables"
)

foreach(bench_src IN LISTS BENCHMARK_SOURCES)
  get_filename_component(bench_name "${bench_src}" NAME_WE)

  add_executable(${bench_name} ${bench_src})
  target_link_libraries(${bench_name} PRIVATE vector_lib benchmark::benchmark benchmark::benchmark_main)
  target_include_directories(${bench_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

  add_dependencies(benchmarks ${bench_name})

  add_custom_target(bench_${bench_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${bench_name}
    DEPENDS ${bench_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running benchmark ${bench_name}"
  )
endforeach()