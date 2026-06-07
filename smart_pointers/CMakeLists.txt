cmake_minimum_required(VERSION 3.20)
project(SmartPointersProject LANGUAGES CXX) # Changed project name

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")

include(FetchContent)
enable_testing()

# Keep GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest) # Removed googlebenchmark

add_library(smart_pointers_lib smart_pointers.cpp)
target_include_directories(smart_pointers_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(smart_pointer_unit_tests unique_ptr_unit_test.cpp)
target_link_libraries(smart_pointer_unit_tests PRIVATE smart_pointers_lib GTest::gtest)

include(GoogleTest)
gtest_discover_tests(smart_pointer_unit_tests)

add_custom_target(check
  COMMAND smart_pointer_unit_tests
  DEPENDS smart_pointer_unit_tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running unit tests"
)