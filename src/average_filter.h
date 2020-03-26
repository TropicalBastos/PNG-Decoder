#ifndef AVERAGE_FILTER_H
#define AVERAGE_FILTER_H

#include <vector>
#include <cmath>

class AverageFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, std::vector<std::vector<uint8_t>>& original, int bpp, int yPos)
    {
        for (int x = (bpp + 1); x < bytes.size(); x++) {
            uint8_t above;
            if (yPos < 1) {
                above = 0;
            } else {
                above = original[yPos - 1][x];
            }

            bytes[x] = std::floor((bytes[x - bpp] + above) / 2);
        }
    }
};

#endif