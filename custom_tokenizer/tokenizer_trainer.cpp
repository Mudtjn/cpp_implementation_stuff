#include <iostream>
#include <string>
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


    std::cout << "-------------------- RUNNING TEST RUN ------------------------\n" ; 
    std::u32string test_string {U"The loop runs while it + 1 < encoded_vec.size(), so the maximum value it ever takes is encoded_vec.size() - 2. That means it == encoded_vec.size() - 1 is never true — that special-case branch is dead code. So whenever the last pair in the vector doesn't happen to match max_it->first, encoded_vec[size-1] (the very last token) never gets pushed into new_encoded_vec at all. It's silently dropped.\n\nSince this loop runs once per merge iteration (up to VOCAB_SIZE - START_VOCAB_SIZE times), you can lose a token almost every pass — which is why your final decoded size doesn't match the original.\n\nFix — rewrite with a while loop and explicit index control, it's much less error-prone than juggling it++ in two places:"};     

    std::vector<uint8_t> test_vec = EncoderDecoder::encode(test_string);
    std::vector<int> test_encoded_vec = tokenizer.encode(test_vec); 

    std::vector<uint8_t> test_decoded_vec = tokenizer.decode(test_encoded_vec);
    EncoderDecoder::print_u32_string(EncoderDecoder::decode(test_decoded_vec)); 

    return 0;
}
