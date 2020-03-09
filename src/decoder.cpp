#include "decoder.h"
#include <iostream>

PNGDecoder::PNGDecoder(char* path)
{
    fs.open(path, std::fstream::in);
}

PNGDecoder::~PNGDecoder()
{
    if (fs.is_open())
        fs.close();
}

std::vector<RGBPixel> PNGDecoder::decode()
{
    if (checkSignature()) {
        printf("PNG signature verified");
    } else {
        std::cerr << "Not a PNG file..." << std::endl;
        return std::vector<RGBPixel>();
    }

    int len = scanNextDataLen();
    while (len != 0) {

        scanNextDataLen();
    }

    return std::vector<RGBPixel>();
}

uint32_t PNGDecoder::scanNextDataLen() {
    uint32_t len;

    if (!fs.eof() && fs.read((char*) &len, sizeof(len))) {
        return len;
    } else {
        return 0;
    }
}

PNG_data_type PNGDecoder::scanNextDataType() {
    char type[4];
    fs.read(type, 4);

    if (strcmp(type, PNG_HEADER_IDENTIFIER) == 0) {
        return PNG_data_type::IHDR;
    } else if (strcmp(type, PNG_DATA_IDENTIFIER) == 0) {
        return PNG_data_type::IDAT;
    }

    return PNG_data_type::NONE;
}

bool PNGDecoder::checkSignature()
{
    // FIXME we need to check that the file size is 8 bytes or over
    fs.read((char*) &signature, sizeof(PNG_signature));
    return (
        signature.entry == 0x89 &&
        signature.p == 0x50 &&
        signature.n == 0x4E &&
        signature.g == 0x47 &&
        signature.dos_line_ending_1 == 0x0D &&
        signature.dos_line_ending_2 == 0x0A &&
        signature.eof == 0x1A &&
        signature.unix_line_ending == 0x0A
    );
}