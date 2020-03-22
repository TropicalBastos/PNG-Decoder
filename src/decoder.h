#ifndef PNG_DECODER_H
#define PNG_DECODER_H

#include "types.h"
#include <vector>
#include <fstream>
#include <string>

class PNGDecoder {
public:
    PNGDecoder(char* path);
    ~PNGDecoder();
    std::vector<RGBPixel> decode();
    uint32_t toLittleEndian(unsigned char* buffer);
private:
    std::fstream fs;
    PNG_signature signature;
    PNG_header hdr;
    std::vector<RGBPixel> pixels;
    std::vector<std::vector<uint32_t>> scanLines;
    unsigned cpos = 12;

    bool checkSignature();
    void scanHeader();
    uint32_t scanNextDataLen();
    PNG_data_type scanNextDataType();
    PNG_data_type scanChunkHdr();
    void readHdr();
    void readAppropriateChunk(PNG_data_type type, uint32_t len);
    std::string readIDATStream(uint32_t len);
    void processScanlines(const std::string buffer);
    template<typename T>
    void unfilterBytes(std::vector<T>& t, int yPos, std::vector<std::vector<T>> original, uint8_t filterMethod, int bpp);
};

#endif