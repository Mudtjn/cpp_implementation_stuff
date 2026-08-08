#include "bpe_tokenizer.hpp"
#include <algorithm>
#include <iostream>

BPETokenizer::BPETokenizer(int vocab_size, int start_vocab_size)
    : vocab_size_(vocab_size), start_vocab_size_(start_vocab_size) {}

void BPETokenizer::train(const std::vector<uint8_t>& bytes) {
    pair_to_new_token_mapping_.clear();
    new_token_to_pair_mapping_.clear();

    std::vector<int> encoded_vec;
    encoded_vec.reserve(bytes.size());
    for (uint8_t b : bytes) {
        encoded_vec.push_back(static_cast<int>(b));
    }

    for (int i = 1; i < (vocab_size_ - start_vocab_size_); i++) {
        if (encoded_vec.size() < 2) {
            break;
        }

        // Get the most common pair
        std::map<std::pair<int, int>, int> pair_frequency;
        for (size_t it = 0; it + 1 < encoded_vec.size(); it++) {
            pair_frequency[{encoded_vec[it], encoded_vec[it + 1]}]++;
        }

        if (pair_frequency.empty()) {
            break;
        }

        auto max_it = std::max_element(
            pair_frequency.begin(), pair_frequency.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            }
        );

        int new_token = start_vocab_size_ + i;
        if (pair_to_new_token_mapping_.find(max_it->first) != pair_to_new_token_mapping_.end()) {
            break;
        } else {
            pair_to_new_token_mapping_[max_it->first] = new_token;
            new_token_to_pair_mapping_[new_token] = max_it->first;
            std::cout << "MOST FREQUENT PAIR: (" << max_it->first.first << ", " << max_it->first.second
                      << ") -> " << max_it->second << " -> assigned number " << new_token << std::endl;
        }

        // Replace the pair with the new token
        std::vector<int> new_encoded_vec;
        new_encoded_vec.reserve(encoded_vec.size());
        for (size_t it = 0; it + 1 < encoded_vec.size(); it++) {
            if (encoded_vec[it] == max_it->first.first && encoded_vec[it + 1] == max_it->first.second) {
                new_encoded_vec.push_back(new_token);
                it++;
            } else {
                new_encoded_vec.push_back(encoded_vec[it]);
                if (it == encoded_vec.size() - 2) {
                    new_encoded_vec.push_back(encoded_vec[it + 1]);
                }
            }
        }
        encoded_vec = std::move(new_encoded_vec);
    }
}

std::vector<int> BPETokenizer::encode(const std::vector<uint8_t>& bytes) const {
    std::vector<int> encoded;
    encoded.reserve(bytes.size());
    for (uint8_t b : bytes) {
        encoded.push_back(static_cast<int>(b));
    }

    // Apply merges in the order they were learned.
    std::vector<std::pair<std::pair<int, int>, int>> merges(pair_to_new_token_mapping_.begin(), pair_to_new_token_mapping_.end());
    std::sort(merges.begin(), merges.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    for (const auto& merge : merges) {
        std::vector<int> new_encoded;
        new_encoded.reserve(encoded.size());
        for (size_t i = 0; i < encoded.size(); ) {
            if (i + 1 < encoded.size() && encoded[i] == merge.first.first && encoded[i + 1] == merge.first.second) {
                new_encoded.push_back(merge.second);
                i += 2;
            } else {
                new_encoded.push_back(encoded[i]);
                i++;
            }
        }
        encoded = std::move(new_encoded);
    }
    return encoded;
}

std::vector<uint8_t> BPETokenizer::decode(const std::vector<int>& tokens) const {
    std::vector<uint8_t> decoded;
    decoded.reserve(tokens.size() * 2);
    for (int t : tokens) {
        decode_token_recursive(t, decoded);
    }
    return decoded;
}

void BPETokenizer::decode_token_recursive(int token, std::vector<uint8_t>& out_result) const {
    if (token <= start_vocab_size_) {
        out_result.push_back(static_cast<uint8_t>(token));
        return;
    }
    auto it = new_token_to_pair_mapping_.find(token);
    if (it == new_token_to_pair_mapping_.end()) {
        out_result.push_back(static_cast<uint8_t>(token & 0xFF));
        return;
    }
    decode_token_recursive(it->second.first, out_result);
    decode_token_recursive(it->second.second, out_result);
}
