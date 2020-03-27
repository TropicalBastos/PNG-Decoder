#ifndef SUB_FILTER_H
#define SUB_FILTER_H

#include <vector>

class SubFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, int bpp)
    {
        for (int x = (bpp + 1); x < bytes.size(); x++) {
            bytes[x] = (bytes[x] + bytes[x - bpp]) % 256;
        }
    }
};

#endif