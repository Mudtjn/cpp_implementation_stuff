#include "encoder_decoder_helper.hpp"
#include <stdexcept>
#include <iostream>

namespace EncoderDecoder {

uint32_t char_to_code_point(char32_t x) {
    return static_cast<uint32_t>(x);
}

char32_t code_point_to_char(uint32_t x) {
    return static_cast<char32_t>(x); 
}

std::vector<uint8_t> encode_utf8(uint32_t cp) {
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

std::vector<uint8_t> encode(std::u32string_view str) {
    std::vector<uint8_t> ans; 
    for (char32_t cp : str) {
        std::vector<uint8_t> bytes = encode_utf8(static_cast<uint32_t>(cp));
        ans.insert(ans.end(), bytes.begin(), bytes.end());
    }
    return ans; 
}

char32_t decode_utf8(const std::vector<uint8_t> &vec, uint32_t &index) {
    if (index >= vec.size()) {
        throw std::runtime_error("Index out of bounds in decode_utf8");
    }

    // get the number of bytes involved in making character
    auto numBytesInCharacter {0}; 
    uint32_t codePoint {};
    if ((vec[index] & 0x80) == 0){ 
        numBytesInCharacter = 1;
        codePoint = vec[index] & 0x7F; 
    } 
    else if ((vec[index] & 0xE0) == 0xC0) {
        numBytesInCharacter = 2;
        codePoint = vec[index] & 0x1F; 
    }
    else if((vec[index] & 0xF0) == 0xE0) {
        numBytesInCharacter = 3;
        codePoint = vec[index] & 0x0F; 
    } 
    else if((vec[index] & 0xF8) == 0xF0) {
        numBytesInCharacter = 4;
        codePoint = vec[index] & 0x07; // Fixed mask from 0x01 to 0x07 to extract all 3 bits of data
    } 
    else {
        throw std::runtime_error("Invalid character format for utf-8"); // Fixed missing throw keyword
    }
    
    for (auto it {1}; it < numBytesInCharacter; it++) {
        if (index + it >= vec.size()) {
            throw std::runtime_error("Truncated UTF-8 sequence"); // Fixed truncation out-of-bounds safety
        }
        uint8_t nextByte = vec[index + it];
        if ((nextByte & 0xC0) != 0x80) {
            throw std::runtime_error("Invalid UTF-8 continuation byte"); // Validation of continuation bytes
        }
        codePoint = (codePoint << 6) | (nextByte & 0x3F); 
    }
    index += numBytesInCharacter; 
    return code_point_to_char(codePoint); 
}

std::u32string decode(const std::vector<uint8_t>& vec) {
    std::u32string ans;
    uint32_t index {0}; 
    while (index < vec.size()) { // Refactored from do-while to while to prevent out-of-bounds access on empty vectors
        ans += decode_utf8(vec, index); 
    }
    return ans;
}

void print_u32_string(std::u32string_view u32_str) {
    // Avoid deprecated and compiler-dependent std::wstring_convert/<codecvt>
    std::vector<uint8_t> bytes = encode(u32_str);
    std::string utf8_str(bytes.begin(), bytes.end());
    std::cout << utf8_str << std::endl;
}

} // namespace EncoderDecoder
