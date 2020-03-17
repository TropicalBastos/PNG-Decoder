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
    // reset inner pixel data
    pixels = std::vector<RGBPixel>();

    if (checkSignature()) {
        std::cout << "PNG signature verified" << std::endl;
    } else {
        std::cerr << "Not a PNG file..." << std::endl;
        return std::vector<RGBPixel>();
    }

    int len = scanNextDataLen();

    while (len != 0) {
        PNG_data_type type = scanChunkHdr();
        if (type == PNG_data_type::NONE) {
            break;
        }
        readAppropriateChunk(type);
        len = scanNextDataLen();
    }

    return pixels;
}

void PNGDecoder::readAppropriateChunk(PNG_data_type type)
{
    switch(type) {
        case PNG_data_type::IHDR: {
            readHdr();
            break;
        }
        case PNG_data_type::IDAT: {
            break;
        }
        case PNG_data_type::NONE:
        default:
            break;
    }
}

void PNGDecoder::readHdr()
{
    fs.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
}

void PNGDecoder::readDataChunk()
{
    return;
}

PNG_data_type PNGDecoder::scanChunkHdr() 
{
    char chunkId[5];
    if (!fs.eof() && fs.read(chunkId, 4) && !fs.fail()) {
        chunkId[4] = '\0';
        if (strcmp(chunkId, "IHDR") == 0) {
            return PNG_data_type::IHDR;
        } else if (strcmp(chunkId, "IDAT")) {
            return PNG_data_type::IDAT;
        } else {
            return PNG_data_type::NONE;
        }
    } else {
        return PNG_data_type::NONE;
    }
}

uint32_t PNGDecoder::scanNextDataLen() 
{
    uint32_t len;
    char buffer[4];

    if (!fs.eof() && fs.read(buffer, sizeof(len)) && !fs.fail()) {
        return toLittleEndian(buffer);
    } else {
        return 0;
    }
}

uint32_t PNGDecoder::toLittleEndian(char* buffer)
{
    return (int)buffer[3] | (int)buffer[2]<<8 | (int)buffer[1]<<16 | (int)buffer[0]<<24;
}

PNG_data_type PNGDecoder::scanNextDataType() 
{
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
    fs.read((char*) &signature, sizeof(PNG_signature));
    if (fs.fail()) return false;
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