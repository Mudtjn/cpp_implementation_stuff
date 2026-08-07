# Tokenizer Design Decisions

This document tracks the technical and design decisions made during the implementation of the custom tokenizer.

---

## 1. Choice of String representation: `std::u32string` over `std::string`

### Decision
For representing and processing input strings in the tokenizer, we use `std::u32string` instead of standard `std::string`.

### Context & Rationale
1. **UTF-8 Multi-byte Issues with `std::string`**: 
   Standard `std::string` stores characters as 8-bit bytes (`char`). In UTF-8, non-ASCII characters (e.g., Hindi `अ` or Japanese `あ`) are encoded using multiple bytes (often 3 bytes per character). This leads to several issues in tokenization:
   - Indexing operations (e.g., `str[i]`) return individual bytes (code units) rather than complete Unicode characters (code points).
   - Iteration and substring operations split multi-byte characters, resulting in malformed or corrupted sequences.
   - Character count is not equivalent to string length (`str.length()`).

2. **Fixed-Width Unicode with `std::u32string`**:
   `std::u32string` is a sequence of `char32_t` (32-bit characters). It represents each Unicode character/code point as a single 32-bit element.
   - **Consistency**: Non-ASCII characters (including Hindi, Japanese, and emojis) are kept as a single, uniform 32-bit element.
   - **Direct Indexing**: Indexing, substring slicing, and matching operations can be performed directly and safely without parsing multi-byte boundaries.
   - **Simpler Merging/Splitting**: In BPE (Byte Pair Encoding) or other tokenization algorithms, merging adjacent characters or splitting words is trivial and structurally safe because every element is a distinct, complete character.

### Impact on Implementation
- Input `std::string` (e.g., UTF-8 encoded text from files/network) must be decoded to `std::u32string` before processing.
- The tokenizer's internal representations (e.g., vocabulary, merges) will operate on `std::u32string` and `char32_t` code points.
- Outputs will be encoded back to standard UTF-8 (`std::string` or `std::vector<uint8_t>`) when decoding is performed.
