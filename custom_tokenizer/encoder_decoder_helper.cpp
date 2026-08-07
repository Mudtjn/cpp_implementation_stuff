#include "encoder_decoder_helper.hpp"

uint32_t EncoderDecoder::code_point_to_number(char32_t x) {
    return static_cast<uint32_t>(x);
}

std::vector<uint8_t> EncoderDecoder::encode_utf8(uint32_t cp) {
    std::vector<uint8_t> bytes;
    if (cp <= 0x7F) {
        bytes.push_back(static_cast<uint8_t>(cp));
    } 
    else if (cp <= 0x7FF) {
        bytes.push_back(static_cast<uint8_t>(0xC0 | (cp >> 6)));
        bytes.push_back(static_cast<uint8_t>(0x80 | (cp & 0x3F)));
    } 
    else if (cp <= 0xFFFF) {
        bytes.push_back(static_cast<uint8_t>(0xE0 | (cp >> 12)));
        bytes.push_back(static_cast<uint8_t>(0x80 | ((cp >> 6) & 0x3F)));
        bytes.push_back(static_cast<uint8_t>(0x80 | (cp & 0x3F)));
    }
    else if (cp <= 0x10FFFF) {
        bytes.push_back(static_cast<uint8_t>(0xF0 | (cp >> 18)));
        bytes.push_back(static_cast<uint8_t>(0x80 | ((cp >> 12) & 0x3F)));
        bytes.push_back(static_cast<uint8_t>(0x80 | ((cp >> 6) & 0x3F)));
        bytes.push_back(static_cast<uint8_t>(0x80 | (cp & 0x3F)));
    }
    return bytes;
}

std::vector<uint8_t> EncoderDecoder::encode(const std::u32string& str) {
    std::vector<uint8_t> ans; 
    for (char32_t cp : str) {
        std::vector<uint8_t> bytes = encode_utf8(static_cast<uint32_t>(cp));
        ans.insert(ans.end(), bytes.begin(), bytes.end());
    }
    return ans; 
}

std::u32string EncoderDecoder::decode(const std::vector<uint8_t>& vec) {
    std::u32string ans;
    size_t i = 0;
    while (i < vec.size()) {
        uint32_t cp = 0;
        uint8_t b1 = vec[i];
        if (b1 <= 0x7F) {
            cp = b1;
            i += 1;
        } else if ((b1 & 0xE0) == 0xC0) {
            if (i + 1 < vec.size()) {
                uint8_t b2 = vec[i + 1];
                cp = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
                i += 2;
            } else {
                break; // Malformed UTF-8, stop decoding
            }
        } else if ((b1 & 0xF0) == 0xE0) {
            if (i + 2 < vec.size()) {
                uint8_t b2 = vec[i + 1];
                uint8_t b3 = vec[i + 2];
                cp = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
                i += 3;
            } else {
                break; // Malformed UTF-8, stop decoding
            }
        } else if ((b1 & 0xF8) == 0xF0) {
            if (i + 3 < vec.size()) {
                uint8_t b2 = vec[i + 1];
                uint8_t b3 = vec[i + 2];
                uint8_t b4 = vec[i + 3];
                cp = ((b1 & 0x07) << 18) | ((b2 & 0x3F) << 12) | ((b3 & 0x3F) << 6) | (b4 & 0x3F);
                i += 4;
            } else {
                break; // Malformed UTF-8, stop decoding
            }
        } else {
            i += 1; // Skip invalid start byte
        }
        ans.push_back(static_cast<char32_t>(cp));
    }
    return ans;
}
