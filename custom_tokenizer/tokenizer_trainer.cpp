#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include "encoder_decoder_helper.hpp"
#include "bpe_tokenizer.hpp"

#define VOCAB_SIZE 300
#define START_VOCAB_SIZE 255

int main() {
    // Read training data from corpus.txt
    std::ifstream file("corpus.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open corpus.txt. Please make sure the file exists." << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string text_utf8 = buffer.str();

    // Convert raw UTF-8 string bytes into std::u32string using our helper
    std::vector<uint8_t> raw_bytes(text_utf8.begin(), text_utf8.end());
    std::u32string corpus = EncoderDecoder::decode(raw_bytes);

    // Encode to UTF-8 bytes representation
    std::vector<uint8_t> vec = EncoderDecoder::encode(corpus);

    // Train BPETokenizer
    BPETokenizer tokenizer(VOCAB_SIZE, START_VOCAB_SIZE);
    tokenizer.train(vec);

    // Encode the corpus
    std::vector<int> encoded_vec = tokenizer.encode(vec);

    std::cout << "---------------------AFTER ENCODING-----------------------\n";
    std::cout << "VEC SIZE: " << vec.size() << std::endl;
    std::cout << "ENCODED_VEC SIZE: " << encoded_vec.size() << std::endl;

    // Decode tokens back to raw bytes
    std::vector<uint8_t> decoded_vec = tokenizer.decode(encoded_vec);

    std::cout << "---------------------AFTER DECODING-----------------------\n";
    std::cout << "DECODED_VEC SIZE: " << decoded_vec.size() << std::endl;

    // Check if the original bytes match the decoded bytes
    if (vec == decoded_vec) {
        std::cout << "SUCCESS: Roundtrip matches perfectly!" << std::endl;
    } else {
        std::cerr << "ERROR: Roundtrip mismatch!" << std::endl;
    }

    return 0;
}
