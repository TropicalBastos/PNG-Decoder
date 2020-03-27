#ifndef PAETH_FILTER_H
#define PAETH_FILTER_H

#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cmath>

class PaethFilter
{
public:
    static inline uint8_t paethPredictor(uint8_t left, uint8_t above, uint8_t upperLeft)
    {
        uint8_t p = (left + above) - upperLeft;
        uint8_t pa = std::abs(p - left);
        uint8_t pb = std::abs(p - above);
        uint8_t pc = std::abs(p - upperLeft);

        if (pa <= pb && pa <= pc) {
            return left;
        } else if (pb <= pc) {
            return above;
        } else {
            return upperLeft;
        }
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