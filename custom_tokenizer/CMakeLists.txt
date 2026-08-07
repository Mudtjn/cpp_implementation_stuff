cmake_minimum_required(VERSION 3.20)
project(CustomTokenizerProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
enable_testing()

# Fetch GoogleTest (identical to other projects in the repository)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

# Library for the encoder/decoder helpers
add_library(custom_tokenizer_lib encoder_decoder_helper.cpp)
target_include_directories(custom_tokenizer_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Unit tests executable
add_executable(custom_tokenizer_unit_tests test/encoder_decoder_test.cpp)
target_link_libraries(custom_tokenizer_unit_tests PRIVATE custom_tokenizer_lib GTest::gtest)

include(GoogleTest)
gtest_discover_tests(custom_tokenizer_unit_tests)

# Target to run tests
add_custom_target(check
  COMMAND custom_tokenizer_unit_tests
  DEPENDS custom_tokenizer_unit_tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Running custom tokenizer unit tests"
)

# Trainer executable
add_executable(tokenizer_trainer tokenizer_trainer.cpp)
