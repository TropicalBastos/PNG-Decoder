#ifndef PNG_DECODER_H
#define PNG_DECODER_H

#include <stdlib.h>
#include <cstdint>
#include <vector>
#include <fstream>

#define PNG_HEADER_IDENTIFIER "IHDR"

struct RGBPixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct PNG_signature
{
    uint8_t entry;
    uint8_t p;
    uint8_t n;
    uint8_t g;
    uint8_t dos_line_ending_1;
    uint8_t dos_line_ending_2;
    uint8_t eof;
    uint8_t unix_line_ending;
};

class PNGDecoder {
public:
    PNGDecoder(char* path);
    ~PNGDecoder();
    std::vector<RGBPixel> decode();
private:
    std::fstream fs;
    PNG_signature signature;
    bool checkSignature();
};

#endif