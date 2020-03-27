#ifndef AVERAGE_FILTER_H
#define AVERAGE_FILTER_H

#include <vector>
#include <cmath>

class AverageFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, std::vector<std::vector<uint8_t>>& original, int bpp, int yPos)
    {
        for (int x = 1; x < bytes.size(); x++) {
            uint8_t above = 0;
            uint8_t left = 0;

            if (yPos >= 1) {
                above = original[yPos - 1][x];
            }

            if (x >= (bpp + 1)) {
                left = bytes[x - bpp];
            }

            bytes[x] = (bytes[x] + (uint8_t) (std::floor((left + above) / 2))) % 256;
        }
    }
};

#endif