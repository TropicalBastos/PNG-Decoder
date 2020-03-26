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
    std::vector<PixelScanline> decode();
    uint32_t toLittleEndian(unsigned char* buffer);
    PNG_header inline getPNGHeader() const { return hdr; }
private:
    std::fstream fs;
    PNG_signature signature;
    PNG_header hdr;
    std::vector<PixelScanline> scanlines;
    unsigned cpos = 12;

    bool checkSignature();
    void scanHeader();
    uint32_t scanNextDataLen();
    PNG_data_type scanNextDataType();
    PNG_data_type scanChunkHdr();
    void readHdr();
    void readAppropriateChunk(PNG_data_type type, uint32_t len);
    std::string readIDATStream(uint32_t len);
    void processScanlines(const std::string& buffer);

    void unfilterBytes(
        std::vector<uint8_t>& bytes,
        std::vector<std::vector<uint8_t>> original, 
        uint8_t filterMethod, 
        int bpp,
        int yPos
    );

    void buildPixels(std::vector<std::vector<uint8_t>> unfilteredBytes, int bpp);
};

#endif