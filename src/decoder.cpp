#include "decoder.h"
#include "./zlib/zlib.h"
#include <iostream>
#include <arpa/inet.h>

#define CHUNK 16384
#define ERROR_STRING std::string("ERROR")
#define CRC_LEN 4
#define DATA_LEN 4
#define FOUR_BYTE_LEN 4
#define SCAN_MOVE_CURSOR cpos += len;\
            cpos += CRC_LEN;\
            fs.seekg(cpos);\
            len = scanNextDataLen();\
            cpos += DATA_LEN;

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
    scanLines = std::vector<std::vector<uint32_t>>();

    if (checkSignature()) {
        std::cout << "PNG signature verified" << std::endl;
    } else {
        std::cerr << "Not a PNG file..." << std::endl;
        return std::vector<RGBPixel>();
    }

    unsigned len = scanNextDataLen();
    int cpos = 12;

    while (len != 0) {
        PNG_data_type type = scanChunkHdr();
        std::cout << "Chunk data length: " << len << std::endl;
        cpos += 4;
        if (type == PNG_data_type::UNKNOWN) {
            SCAN_MOVE_CURSOR
            continue;
        }

        if (type == PNG_data_type::IEND) {
            break;
        }

        readAppropriateChunk(type, len);
        SCAN_MOVE_CURSOR
    }

    if (scanLines.size() == 0) {
        return pixels;
    }

    return pixels;
}

std::string PNGDecoder::decompressChunk(unsigned char* data, uint32_t len)
{
    int ret;
    unsigned have;
    z_stream strm;

    unsigned char in[len];
    unsigned char out[len];

    std::string decompressed;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ERROR_STRING;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = len;
        if (strm.avail_in == 0)
            break;
        strm.next_in = data;

         /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = len;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ERROR_STRING;
            }

            have = len - strm.avail_out;
            decompressed.append(reinterpret_cast<char*>(out), have);
            
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    if (ret == Z_STREAM_END) {
        return decompressed;
    } else {
        return ERROR_STRING;
    }
}

void PNGDecoder::readIDATChunk(uint32_t len)
{
    std::cout << len << std::endl;
    unsigned char in[len];
    // fs.read(reinterpret_cast<char*>(in), len);
    // std::string decompressed = decompressChunk(in, len);
    // std::cout << decompressed << std::endl;
}

void PNGDecoder::readAppropriateChunk(PNG_data_type type, uint32_t len)
{
    switch(type) {
        case PNG_data_type::IHDR: {
            readHdr();
            break;
        }
        case PNG_data_type::IDAT: {
            readIDATChunk(len);
            break;
        }
        default:
            break;
    }
}

void PNGDecoder::readHdr()
{
    fs.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    hdr.height = ntohl(hdr.height);
    hdr.width = ntohl(hdr.width);
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
        std::cout << "Chunk Type: " << chunkId << std::endl;
        if (strcmp(chunkId, "IHDR") == 0) {
            return PNG_data_type::IHDR;
        } else if (strcmp(chunkId, "IDAT") == 0) {
            return PNG_data_type::IDAT;
        } else if (strcmp(chunkId, "IEND") == 0) {
            return PNG_data_type::IEND;
        } else {
            return PNG_data_type::UNKNOWN;
        }
    } else {
        return PNG_data_type::UNKNOWN;
    }
}

uint32_t PNGDecoder::scanNextDataLen() 
{
    uint32_t len;
    unsigned char buffer[4];

    if (!fs.eof() && fs.read(reinterpret_cast<char*>(buffer), sizeof(len)) && !fs.fail()) {
        return toLittleEndian(buffer);
    } else {
        return 0;
    }
}

uint32_t PNGDecoder::toLittleEndian(unsigned char* buffer)
{
    return (uint32_t)buffer[3] | (uint32_t)buffer[2]<<8 | (uint32_t)buffer[1]<<16 | (uint32_t)buffer[0]<<24;
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

    return PNG_data_type::UNKNOWN;
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