#ifndef UP_FILTER_H
#define UP_FILTER_H

#include <vector>

class UpFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, std::vector<std::vector<uint8_t>>& original, int yPos)
    {
        for (int x = 1; x < bytes.size(); x++) {
            uint8_t above;
            if (yPos < 1) {
                above = 0;
            } else {
                above = original[yPos - 1][x];
            }

            bytes[x] = bytes[x] + above;
        }
    }
};

#endif