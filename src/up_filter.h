#ifndef UP_FILTER_H
#define UP_FILTER_H

#include <vector>

class UpFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, std::vector<std::vector<uint8_t>>& original)
    {
        for (int y = 1; y < original.size(); y++) {
            for (int x = 1; x < bytes.size(); x++) {
                bytes[x] = bytes[x] + original[y - 1][x];
            }
        }
    }
};

#endif