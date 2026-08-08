#include "bpe_tokenizer.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(BPETokenizerTest, TrainAndRoundTripBasic) {
    BPETokenizer tokenizer(260, 255); // target vocab size 260, start 255 (allowing 5 merges)
    std::vector<uint8_t> input = {'a', 'b', 'c', 'a', 'b', 'c', 'd'};
    tokenizer.train(input);

    std::vector<int> encoded = tokenizer.encode(input);
    std::vector<uint8_t> decoded = tokenizer.decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(BPETokenizerTest, TrainEmptyInput) {
    BPETokenizer tokenizer(300, 255);
    std::vector<uint8_t> input;
    EXPECT_NO_THROW(tokenizer.train(input));

    std::vector<int> encoded = tokenizer.encode(input);
    EXPECT_TRUE(encoded.empty());

    std::vector<uint8_t> decoded = tokenizer.decode(encoded);
    EXPECT_TRUE(decoded.empty());
}

TEST(BPETokenizerTest, SingleCharacterInput) {
    BPETokenizer tokenizer(300, 255);
    std::vector<uint8_t> input = {'a', 'a', 'a'};
    tokenizer.train(input);

    std::vector<int> encoded = tokenizer.encode(input);
    std::vector<uint8_t> decoded = tokenizer.decode(encoded);

    EXPECT_EQ(decoded, input);
}

TEST(BPETokenizerTest, UnseenInputAfterTraining) {
    BPETokenizer tokenizer(260, 255);
    std::vector<uint8_t> train_input = {'a', 'b', 'a', 'b', 'a', 'b'};
    tokenizer.train(train_input);

    // Encode something that has seen pairs and unseen characters
    std::vector<uint8_t> test_input = {'a', 'b', 'z', 'a', 'b'};
    std::vector<int> encoded = tokenizer.encode(test_input);
    std::vector<uint8_t> decoded = tokenizer.decode(encoded);

    EXPECT_EQ(decoded, test_input);
}
