cmake_minimum_required(VERSION 3.20)
project(SmartPointersProject LANGUAGES CXX) # Changed project name

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
enable_testing()

# Keep GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest) # Removed googlebenchmark

# Add your test executable directly
# (Assuming your test file is named smart_pointers_test.cpp)
add_executable(SmartPointerTests unique_ptr_unit_test.cpp)

# Link directly to GTest and allow include access to the current directory
add_library(smart_pointer_lib smart_pointers.hpp)
target_include_directories(smart_pointer_lib PUBLIC ${CURRENT_CMAKE_SOURCE_DIR})

target_link_libraries(SmartPointerTests PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(SmartPointerTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Automatically discover the GTest cases
include(GoogleTest)
gtest_discover_tests(SmartPointerTests)
