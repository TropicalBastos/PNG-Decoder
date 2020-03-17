#ifndef PNG_DECODER_H
#define PNG_DECODER_H

#include <stdlib.h>
#include <cstdint>
#include <vector>
#include <fstream>

#define PNG_HEADER_IDENTIFIER "IHDR"
#define PNG_DATA_IDENTIFIER "IDAT"

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

struct PNG_header
{
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
    uint8_t crc;
};

enum PNG_data_type
{
    NONE,
    IHDR,
    IDAT,
    IEND
};

class PNGDecoder {
public:
    PNGDecoder(char* path);
    ~PNGDecoder();
    std::vector<RGBPixel> decode();
    uint32_t toLittleEndian(char* buffer);
private:
    std::fstream fs;
    PNG_signature signature;
    PNG_header hdr;
    std::vector<RGBPixel> pixels;
    std::vector<std::vector<uint32_t>> scanLines;

    bool checkSignature();
    void scanHeader();
    uint32_t scanNextDataLen();
    PNG_data_type scanNextDataType();
    PNG_data_type scanChunkHdr();
    void readHdr();
    void readAppropriateChunk(PNG_data_type type);
    void readDataChunk();
};

#endif