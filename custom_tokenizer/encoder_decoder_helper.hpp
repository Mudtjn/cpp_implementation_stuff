#ifndef ENCODER_DECODER_HELPER_HPP
#define ENCODER_DECODER_HELPER_HPP

#include <cstdint>
#include <string>
#include <vector>

class EncoderDecoder {
private:
    uint32_t code_point_to_number(char32_t x);

public:
    std::vector<uint8_t> encode_utf8(uint32_t cp);
    std::vector<uint8_t> encode(const std::u32string& str);
    std::u32string decode(const std::vector<uint8_t>& vec);
};

#endif // ENCODER_DECODER_HELPER_HPP
