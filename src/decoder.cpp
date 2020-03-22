#include "decoder.h"
#include "./zlib/zlib.h"
#include <iostream>
#include <arpa/inet.h>
#include "sub_filter.h"

#define CHUNK 16384
#define ERROR_STRING std::string("ERROR")
#define CRC_LEN 4
#define CHUNK_TYPE_LEN 4
#define FOUR_BYTE_LEN 4
#define SCAN_MOVE_CURSOR cpos += len;\
            cpos += CRC_LEN;\
            fs.seekg(cpos);\
            len = scanNextDataLen();\
            cpos += FOUR_BYTE_LEN;

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
    cpos = 12;

    while (len != 0) {
        PNG_data_type type = scanChunkHdr();
        std::cout << "Chunk data length: " << len << std::endl;
        cpos += CHUNK_TYPE_LEN;

        if (type == PNG_data_type::UNKNOWN) {
            SCAN_MOVE_CURSOR
            continue;
        }

        if (type == PNG_data_type::IEND) {
            break;
        }

        readAppropriateChunk(type, len);

        if (type == PNG_data_type::IHDR) {
            printf("Width: %d | Height: %d\n", hdr.width, hdr.height);
            printf("Color type: %d | Bit depth: %d | Filter method: %d\n", hdr.color_type, hdr.bit_depth, hdr.filter_method);
        }

        // if last known type was IDAT then it did its own stream processing
        // which means we don't need to invoke the entire SCAN_MOVE_CURSOR macro
        if (type == PNG_data_type::IDAT) {
            len = scanNextDataLen();
            cpos += FOUR_BYTE_LEN;
        } else {
            SCAN_MOVE_CURSOR
        }
    }

    if (scanLines.size() == 0) {
        return pixels;
    }

    return pixels;
}

template<typename T>
void PNGDecoder::unfilterBytes(std::vector<T>& t, int yPos, std::vector<std::vector<T>> original, uint8_t filterMethod, int bpp)
{
    switch (filterMethod) {
        case PNG_filter_type::SUB:
            SubFilter::decode(t, bpp);
            break;

        case PNG_filter_type::AVERAGE:
            break;

        case PNG_filter_type::UP:
            break;
        
        case PNG_filter_type::PAETH:
            break;

        default:
            break;
    }
}

void PNGDecoder::processScanlines(const std::string buffer)
{

    switch (hdr.color_type) {
        case PNG_color_type::RGB:
        case PNG_color_type::RGBA: {
            if (hdr.bit_depth == 8) {

                int multiplier = hdr.color_type == PNG_color_type::RGB ? 3 : 4;
                std::vector<std::vector<uint8_t>> unfilteredBytes;
                int cursor = 0;

                for (int scanline = 0; scanline < hdr.height; scanline++) {
                    uint8_t filter_byte = buffer[cursor];
                    cursor++;
                    std::vector<uint8_t> rowBytes;
                    rowBytes.push_back(filter_byte);

                    for (int column = 0; column < (hdr.width * multiplier); column++) {
                        int pos = cursor + column;
                        if (pos < buffer.size()) {
                            rowBytes.push_back(static_cast<uint8_t>(buffer[pos]));
                        }
                        cursor++;
                    }

                    unfilterBytes(rowBytes, scanline, unfilteredBytes, filter_byte, multiplier);
                    unfilteredBytes.push_back(rowBytes);
                }

            } else if (hdr.bit_depth == 16) {
                
                int multiplier = hdr.color_type == PNG_color_type::RGB ? 3 : 4;
                std::vector<std::vector<uint16_t>> unfilteredBytes;
                int cursor = 0;

                for (int scanline = 0; scanline < hdr.height; scanline++) {
                    uint16_t filter_byte = static_cast<uint16_t>(buffer[cursor]);
                    cursor++;
                    std::vector<uint16_t> rowBytes;
                    rowBytes.push_back(filter_byte);

                    for (int column = 0; column < (hdr.width * multiplier); column++) {
                        int pos = cursor + column;
                        int pos2 = pos + 1;
                        if (pos2 < buffer.size()) {
                            rowBytes.push_back(static_cast<uint16_t>(
                                (buffer[pos] << 8) | buffer[pos2])
                            );
                        }
                        cursor += 2;
                    }

                    unfilterBytes(rowBytes, scanline, unfilteredBytes, filter_byte, multiplier * 2);
                    unfilteredBytes.push_back(rowBytes);
                }

            }
        }
        
        default:
            break;
    }
}

std::string PNGDecoder::readIDATStream(uint32_t len)
{
    uint32_t mutableLen = len;
    int ret;
    unsigned bytes_written;
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
    if (ret != Z_OK) {
        std::cerr << "Could not initialize zlib stream" << std::endl;
        return ERROR_STRING;
    }

    do {
        strm.avail_in = mutableLen;
        if (strm.avail_in == 0)
            return decompressed;

        fs.read(reinterpret_cast<char*>(in), mutableLen);
        strm.next_in = in;

            /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = mutableLen;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                    std::cerr << "Data error" << std::endl;
                    (void)inflateEnd(&strm);
                    return ERROR_STRING;
                case Z_MEM_ERROR:
                    std::cerr << "Memory error" << std::endl;
                    (void)inflateEnd(&strm);
                    return ERROR_STRING;
            }

            // avail_out will be amount of bytes free in the output buffer
            // so the amount of bytes written would be the length of the buffer - bytes free
            bytes_written = mutableLen - strm.avail_out;
            decompressed.append(reinterpret_cast<char*>(out), bytes_written);
            
        } while (strm.avail_out == 0);

        cpos += mutableLen;
        cpos += CRC_LEN;
        fs.seekg(cpos);
        mutableLen = scanNextDataLen();
        cpos += FOUR_BYTE_LEN;
        PNG_data_type type = scanChunkHdr();
        cpos += CHUNK_TYPE_LEN;

        if (type == PNG_data_type::IDAT) {
            // seek to next IDAT chunk stream
            std::cout << "IDAT chunk length: " << mutableLen << std::endl;
            fs.seekg(cpos);
        } else {
            if (ret != Z_STREAM_END) {
                std::cerr << "Reached end of IDAT chunks before end of decompression stream" << std::endl;
                (void)inflateEnd(&strm);
                return ERROR_STRING;
            } else if (ret == Z_STREAM_END) {
                // onto next data type
                std::cout << "End of IDAT stream, moving on..." << std::endl;
                cpos -= CHUNK_TYPE_LEN;
                cpos -= FOUR_BYTE_LEN;
                fs.seekg(cpos);
            }
        }

    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    if (ret == Z_STREAM_END) {
        return decompressed;
    }

    std::cerr << "Error with finalizing decompression stream" << std::endl;
    return ERROR_STRING;
}

void PNGDecoder::readAppropriateChunk(PNG_data_type type, uint32_t len)
{
    switch(type) {
        case PNG_data_type::IHDR: {
            readHdr();
            break;
        }
        case PNG_data_type::IDAT: {
            std::string decompressed = readIDATStream(len);
            processScanlines(decompressed);
            std::cout << "Scanlines processed" << std::endl;
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