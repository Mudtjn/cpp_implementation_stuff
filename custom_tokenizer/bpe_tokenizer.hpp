#ifndef BPE_TOKENIZER_HPP
#define BPE_TOKENIZER_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <utility>

class BPETokenizer {
public:
    BPETokenizer(int vocab_size = 300, int start_vocab_size = 255);

    // Train on a given raw byte vector
    void train(const std::vector<uint8_t>& bytes);

    // Encode raw byte vector to tokens using trained rules
    std::vector<int> encode(const std::vector<uint8_t>& bytes) const;

    // Decode tokens back to raw bytes
    std::vector<uint8_t> decode(const std::vector<int>& tokens) const;

private:
    int vocab_size_;
    int start_vocab_size_;
    std::map<std::pair<int, int>, int> pair_to_new_token_mapping_;
    std::map<int, std::pair<int, int>> new_token_to_pair_mapping_;

    // Optimized recursive decode helper
    void decode_token_recursive(int token, std::vector<uint8_t>& out_result) const;
};

#endif // BPE_TOKENIZER_HPP
