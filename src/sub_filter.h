#ifndef SUB_FILTER_H
#define SUB_FILTER_H

#include <vector>

class SubFilter
{
public:
    static inline void decode(std::vector<uint8_t>& bytes, int bpp)
    {
        int x = bpp + 1;
        for (; x < bytes.size(); ++x) {
            bytes[x] = bytes[x] + bytes[x - bpp];
        }
    }
};

#endif