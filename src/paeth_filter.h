#ifndef PAETH_FILTER_H
#define PAETH_FILTER_H

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cmath>

class PaethFilter
{
public:
    static inline uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c)
    {
        short pa = std::abs(b - c);
        short pb = std::abs(a - c);
        short pc = std::abs(a + b - c - c);
        /* return input value associated with smallest of pa, pb, pc (with certain priority if equal) */
        if (pb < pa) { a = b; pa = pb; }
        return (pc < pa) ? c : a;
    }

    static inline void decode(std::vector<uint8_t>& bytes, std::vector<std::vector<uint8_t>>& original, int bpp, int yPos)
    {
        for (int x = 1; x < bytes.size(); x++) {
            uint8_t above = 0;
            uint8_t upperLeft = 0;
            uint8_t left = 0;

            if (yPos >= 1) {
                above = original[yPos - 1][x];
            }

            if (x >= (bpp + 1)) {
                if (yPos >= 1) {
                    upperLeft = original[yPos - 1][x - bpp];
                }
                left = bytes[x - bpp];
            }

            bytes[x] = (bytes[x] + paethPredictor(left, above, upperLeft)) % 256;
        }
    }
};

#endif