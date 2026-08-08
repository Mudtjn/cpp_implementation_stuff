#include "encoder_decoder_helper.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <stdexcept>

TEST(EncoderDecoderTest, EncodeUTF8_ASCII) {
    EncoderDecoder ed;
    std::vector<uint8_t> result = ed.encode_utf8(65);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 0x41);
}

TEST(EncoderDecoderTest, EncodeUTF8_TwoBytes) {
    EncoderDecoder ed;
    std::vector<uint8_t> result = ed.encode_utf8(353);
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0xC5);
    EXPECT_EQ(result[1], 0xA1);
}

TEST(EncoderDecoderTest, EncodeUTF8_ThreeBytes) {
    EncoderDecoder ed;
    std::vector<uint8_t> result = ed.encode_utf8(2309);
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0xE0);
    EXPECT_EQ(result[1], 0xA4);
    EXPECT_EQ(result[2], 0x85);

    std::vector<uint8_t> result2 = ed.encode_utf8(12354);
    ASSERT_EQ(result2.size(), 3);
    EXPECT_EQ(result2[0], 0xE3);
    EXPECT_EQ(result2[1], 0x81);
    EXPECT_EQ(result2[2], 0x82);
}

TEST(EncoderDecoderTest, EncodeUTF8_FourBytes) {
    EncoderDecoder ed;
    std::vector<uint8_t> result = ed.encode_utf8(128512);
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], 0xF0);
    EXPECT_EQ(result[1], 0x9F);
    EXPECT_EQ(result[2], 0x98);
    EXPECT_EQ(result[3], 0x80);
}

TEST(EncoderDecoderTest, EncodeU32String) {
    EncoderDecoder ed;
    std::u32string input = U"Aあš😀";
    std::vector<uint8_t> expected = {
        0x41,
        0xE3, 0x81, 0x82,
        0xC5, 0xA1,
        0xF0, 0x9F, 0x98, 0x80
    };
    std::vector<uint8_t> result = ed.encode(input);
    EXPECT_EQ(result, expected);
}

TEST(EncoderDecoderTest, DecodeU32String) {
    EncoderDecoder ed;
    std::vector<uint8_t> bytes = {
        0x41,
        0xE3, 0x81, 0x82,
        0xC5, 0xA1,
        0xF0, 0x9F, 0x98, 0x80
    };
    std::u32string expected = U"Aあš😀";
    std::u32string result = ed.decode(bytes);
    EXPECT_EQ(result, expected);
}

TEST(EncoderDecoderTest, DecodeEmptyVector) {
    EncoderDecoder ed;
    std::vector<uint8_t> bytes;
    std::u32string result = ed.decode(bytes);
    EXPECT_TRUE(result.empty());
}

TEST(EncoderDecoderTest, HighPlaneUnicodeMaskBugfixTest) {
    EncoderDecoder ed;
    // Test a very high Unicode code point to ensure the 4-byte mask extracts all bits correctly.
    // Highest valid Unicode code point: 0x10FFFF
    std::u32string original = U"\x10FFFF";
    std::vector<uint8_t> encoded = ed.encode(original);
    
    // Ensure it compiles and reconstructs correctly
    std::u32string decoded = ed.decode(encoded);
    EXPECT_EQ(decoded, original);
    EXPECT_EQ(static_cast<uint32_t>(decoded[0]), 0x10FFFF);
}

TEST(EncoderDecoderTest, DecodeInvalidStartByteThrows) {
    EncoderDecoder ed;
    // 0x80 is an invalid UTF-8 start byte (continuation byte pattern)
    std::vector<uint8_t> invalidBytes = {0x80};
    EXPECT_THROW(ed.decode(invalidBytes), std::runtime_error);
    
    // 0xFF is completely invalid in UTF-8
    std::vector<uint8_t> invalidBytes2 = {0xFF};
    EXPECT_THROW(ed.decode(invalidBytes2), std::runtime_error);
}

TEST(EncoderDecoderTest, DecodeTruncatedSequenceThrows) {
    EncoderDecoder ed;
    // 3-byte character start 'あ' (0xE3, 0x81, 0x82) but only provide 0xE3, 0x81
    std::vector<uint8_t> truncatedBytes = {0xE3, 0x81};
    EXPECT_THROW(ed.decode(truncatedBytes), std::runtime_error);

    // 4-byte character start '😀' (0xF0, 0x9F, 0x98, 0x80) but only provide 0xF0
    std::vector<uint8_t> truncatedBytes2 = {0xF0};
    EXPECT_THROW(ed.decode(truncatedBytes2), std::runtime_error);
}

TEST(EncoderDecoderTest, DecodeInvalidContinuationByteThrows) {
    EncoderDecoder ed;
    // 2-byte character 'š' (0xC5, 0xA1), but second byte is invalid (should be 10xxxxxx, i.e., 0x80 to 0xBF)
    // 0xC0 starts with 110xxxxx, so it's not a continuation byte!
    std::vector<uint8_t> invalidContinuation = {0xC5, 0xC0};
    EXPECT_THROW(ed.decode(invalidContinuation), std::runtime_error);
}

TEST(EncoderDecoderTest, DecodeOutOfBoundsCheck) {
    EncoderDecoder ed;
    std::vector<uint8_t> empty;
    uint32_t index = 0;
    // Directly calling decode_utf8 on empty vector at index 0 should throw bounds exception
    EXPECT_THROW(ed.decode_utf8(empty, index), std::runtime_error);
}

TEST(EncoderDecoderTest, RoundTripIdentityComplex) {
    EncoderDecoder ed;
    std::u32string original = U"Hello, World! अあ😀 Let's test custom tokenizer. \U0001F917 \x10FFFD";
    std::vector<uint8_t> encoded = ed.encode(original);
    std::u32string decoded = ed.decode(encoded);
    EXPECT_EQ(decoded, original);
}

TEST(EncoderDecoderTest, PrintU32StringDoesNotCrash) {
    EncoderDecoder ed;
    std::u32string input = U"Testing print: Hello अあ😀!";
    // This should print to console without throwing or crashing
    EXPECT_NO_THROW(ed.print_u32_string(input));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
