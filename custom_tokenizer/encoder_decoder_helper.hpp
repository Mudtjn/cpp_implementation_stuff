#ifndef ENCODER_DECODER_HELPER_HPP
#define ENCODER_DECODER_HELPER_HPP

#include <cstdint>
#include <string>
#include <vector>

class EncoderDecoder {
private:
    uint32_t char_to_code_point(char32_t x);
    char32_t code_point_to_char(uint32_t x);

public:
    std::vector<uint8_t> encode_utf8(uint32_t cp);
    char32_t decode_utf8(const std::vector<uint8_t>& vec, uint32_t &index);
    std::vector<uint8_t> encode(const std::u32string& str);
    std::u32string decode(const std::vector<uint8_t>& vec);
    void print_u32_string(std::u32string u32_str); 
};

#endif // ENCODER_DECODER_HELPER_HPP
