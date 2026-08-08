# Custom BPE Tokenizer

A highly efficient, modular, and robust Byte-Pair Encoding (BPE) Tokenizer implementation in modern C++ (C++23), designed to handle UTF-8 sequences safely and perform tokenization/detokenization logic dynamically.

## Project Structure

- `encoder_decoder_helper.hpp`/`.cpp`: Contains the stateless `EncoderDecoder` namespace which safely handles UTF-8 encode and decode functions.
- `bpe_tokenizer.hpp`/`.cpp`: Implements the `BPETokenizer` class, handling vocabulary training, token encoding, and optimized reference-based token decoding.
- `tokenizer_trainer.cpp`: The driver program that loads an external training corpus, trains the BPE tokenizer, and runs a validation roundtrip.
- `corpus.txt`: The text file containing the raw training corpus data.
- `test/`:
  - `encoder_decoder_test.cpp`: GoogleTest unit tests for UTF-8 encodings, boundary cases, and invalid sequence checks.
  - `bpe_tokenizer_test.cpp`: GoogleTest unit tests for `BPETokenizer` training, unseen token fallbacks, and identity roundtrips.

---

## Build Prerequisites

Ensure you have the following installed on your machine:
- **CMake** (v3.20 or higher)
- **C++ Compiler** with C++23 support (such as GCC 13+ or Clang 16+)
- **Make** build system

GoogleTest is managed and fetched automatically at configure time via CMake's `FetchContent`.

---

## Building the Project

Follow these steps to configure and build both the unit tests and the training executable:

1. Create a `build` directory:
   ```bash
   mkdir -p build && cd build
   ```

2. Configure the project:
   ```bash
   cmake ..
   ```

3. Build the targets:
   ```bash
   make
   ```

This will produce two main executables in the `build/` directory:
- `tokenizer_trainer`: The BPE training driver program.
- `custom_tokenizer_unit_tests`: The GoogleTest unit test runner.

---

## Running the Tokenizer Trainer

The `tokenizer_trainer` executable trains a BPE tokenizer on `corpus.txt` (which is copied into your build folder automatically) and validates that the encoding and decoding process is fully lossless.

Run it from the `build` directory:
```bash
./tokenizer_trainer
```

### Sample Output
When executed, the program outputs:
1. Every step of the training merge process, showing the most frequent byte pairs and their newly assigned token IDs.
2. The comparison size of the raw byte vector versus the tokenized vector (showing compression rates).
3. Verification that detokenization (decoding) perfectly restores the original corpus:
   ```text
   ...
   MOST FREQUENT PAIR: (109, 271) -> 11 -> assigned number 298
   MOST FREQUENT PAIR: (105, 287) -> 10 -> assigned number 299
   ---------------------AFTER ENCODING-----------------------
   VEC SIZE: 2895
   ENCODED_VEC SIZE: 1898
   ---------------------AFTER DECODING-----------------------
   DECODED_VEC SIZE: 2895
   SUCCESS: Roundtrip matches perfectly!
   ```

---

## Running Unit Tests

To run the complete suite of automated unit tests:

Run the test executable from the `build` directory:
```bash
./custom_tokenizer_unit_tests
```

Or run them via CMake's test runner:
```bash
ctest --output-on-failure
```
