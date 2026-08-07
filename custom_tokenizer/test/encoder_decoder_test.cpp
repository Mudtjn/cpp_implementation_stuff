#include "../encoder_decoder_helper.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <string>

TEST(EncoderDecoderTest, EncodeUTF8_ASCII) {
    EncoderDecoder ed;
    // 'A' (Decimal: 65, Hex: 0x41)
    std::vector<uint8_t> result = ed.encode_utf8(65);
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 0x41);
}

TEST(EncoderDecoderTest, EncodeUTF8_TwoBytes) {
    EncoderDecoder ed;
    // Latin Small Letter S with Caron 'š' (Decimal: 353, Hex: 0x0161)
    // UTF-8 bytes: 0xC5 0xA1
    std::vector<uint8_t> result = ed.encode_utf8(353);
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0xC5);
    EXPECT_EQ(result[1], 0xA1);
}

TEST(EncoderDecoderTest, EncodeUTF8_ThreeBytes) {
    EncoderDecoder ed;
    // Hindi Character 'अ' (Decimal: 2309, Hex: 0x0905)
    // UTF-8 bytes: 0xE0 0xA4 0x85
    std::vector<uint8_t> result = ed.encode_utf8(2309);
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 0xE0);
    EXPECT_EQ(result[1], 0xA4);
    EXPECT_EQ(result[2], 0x85);

    // Japanese Character 'あ' (Decimal: 12354, Hex: 0x3042)
    // UTF-8 bytes: 0xE3 0x81 0x82
    std::vector<uint8_t> result2 = ed.encode_utf8(12354);
    ASSERT_EQ(result2.size(), 3);
    EXPECT_EQ(result2[0], 0xE3);
    EXPECT_EQ(result2[1], 0x81);
    EXPECT_EQ(result2[2], 0x82);
}

TEST(EncoderDecoderTest, EncodeUTF8_FourBytes) {
    EncoderDecoder ed;
    // Emoji '😀' (Decimal: 128512, Hex: 0x1F600)
    // UTF-8 bytes: 0xF0 0x9F 0x98 0x80
    std::vector<uint8_t> result = ed.encode_utf8(128512);
    ASSERT_EQ(result.size(), 4);
    EXPECT_EQ(result[0], 0xF0);
    EXPECT_EQ(result[1], 0x9F);
    EXPECT_EQ(result[2], 0x98);
    EXPECT_EQ(result[3], 0x80);
}

TEST(EncoderDecoderTest, EncodeU32String) {
    EncoderDecoder ed;
    // "Aあš😀"
    // 'A' (65, 1 byte)
    // 'あ' (12354, 3 bytes: 0xE3, 0x81, 0x82)
    // 'š' (353, 2 bytes: 0xC5, 0xA1)
    // '😀' (128512, 4 bytes: 0xF0, 0x9F, 0x98, 0x80)
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

TEST(EncoderDecoderTest, RoundTripIdentity) {
    EncoderDecoder ed;
    std::u32string original = U"Hello, World! अあ😀 Let's test custom tokenizer.";
    std::vector<uint8_t> encoded = ed.encode(original);
    std::u32string decoded = ed.decode(encoded);
    EXPECT_EQ(decoded, original);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
