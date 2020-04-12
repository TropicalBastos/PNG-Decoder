#ifndef PNG_DECODER_TYPES_H
#define PNG_DECODER_TYPES_H

#include <stdlib.h>
#include <cstdint>
#include <vector>

#define PNG_HEADER_IDENTIFIER "IHDR"
#define PNG_DATA_IDENTIFIER "IDAT"

struct RGBPixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

typedef std::vector<RGBPixel> PixelScanline;

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
};

enum PNG_data_type
{
    UNKNOWN,
    IHDR,
    PLTE,
    IDAT,
    IEND
};

enum PNG_color_type
{
    GRAYSCALE,
    PADDING_1,
    RGB,
    PLTE_TYPE,
    GRAYSCALE_ALPHA,
    PADDING_2,
    RGBA
};

enum PNG_filter_type
{
    NONE,
    SUB,
    UP,
    AVERAGE,
    PAETH
};

/**
 Color    Allowed    Interpretation
   Type    Bit Depths
   
   0       1,2,4,8,16  Each pixel is a grayscale sample.
   
   2       8,16        Each pixel is an R,G,B triple.
   
   3       1,2,4,8     Each pixel is a palette index;
                       a PLTE chunk must appear.
   
   4       8,16        Each pixel is a grayscale sample,
                       followed by an alpha sample.
   
   6       8,16        Each pixel is an R,G,B triple,
                       followed by an alpha sample.
*/

struct GrayscalePixel8Bit {
    uint8_t grayscale;
};

struct GrayscalePixel16Bit {
    uint16_t grayscale;
};

struct GrayscalePixelAlpha8Bit {
    uint8_t grayscale;
    uint8_t alpha;
};

struct GrayscalePixelAlpha16Bit {
    uint16_t grayscale;
    uint16_t alpha;
};

struct RGBPixel8Bit {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct RGBPixel16Bit {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
};

struct RGBPixelAlpha8Bit {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct RGBPixelAlpha16Bit {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t alpha;
};

#endif