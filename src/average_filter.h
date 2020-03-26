#ifndef AVERAGE_FILTER_H
#define AVERAGE_FILTER_H

#include <vector>
#include <cmath>

class AverageFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, std::vector<std::vector<uint8_t>>& original, int bpp)
    {
        for (int y = 1; y < original.size(); y++) {
            for (int x = (bpp + 1); x < bytes.size(); x++) {
                bytes[x] = std::floor((bytes[x - bpp] + original[y - 1][x]) / 2);
            }
        }
    }
};

#endif