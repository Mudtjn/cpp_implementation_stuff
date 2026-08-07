#include <iostream>
#include "encoder_decoder_helper.cpp"

int main() {
    EncoderDecoder encoderDecoder {}; 
    auto vec {encoderDecoder.encode(U"UTF-8 एनकोडिंग के लिए") };

    std::cout << "["; 
    for(auto x: vec) std::cout << static_cast<int>(x) << ", " ;  
    std::cout << "]" << std::endl; 

    return 0; 
} 