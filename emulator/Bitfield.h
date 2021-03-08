#pragma once

#include "Config.h"

inline std::string to_binary_string(u8 data)
{
    std::string str;
    u8 mask = 0x80;
    for(int i = 0; i < 8; i++)
    {
        str += (data & mask ? "1" : "0");
        mask >>= 1;
    }
    return str;
}

class Bitfield
{
public:
    Bitfield(u8 data)
    : m_data(data) {}

    static u8 flip(u8 data, u8 size = 8)
    {
        u8 ndata = 0x0;
        for(size_t s = 0; s < size; s++)
            ndata |= ((data & (0x1 << s)) >> s) << (size-1 - s);
        //std::cerr << "=" << to_binary_string(ndata) << std::endl;
        return ndata;
    }


    // Get bytes, starting from LSB (right in notation)
    static u8 sub_lsb(u8 data, size_t start, size_t len)
    {
        //std::cerr << "sub_lsb: " << start << ":" << len << std::endl;
        u8 mask = 0xFF;

        mask >>= 8-len;
        mask <<= start;
        u8 ret = (data & mask) >> start;
        //std::cerr << "sub_lsb: " << to_binary_string(ret) << std::endl;
        return ret;
    }

    // Get bytes, starting from MSB (left in notation)
    static u8 sub_msb(u8 data, size_t start, size_t len)
    {
        // We must to flip the data to maintain valid bit order!
        return flip(sub_lsb(flip(data), start, len), len);
    }


    // Get bytes, starting from MSB (left in notation)
    u8 sub_msb(size_t start, size_t len) const
    {
        // We must to flip the data to maintain valid bit order!
        return sub_msb(m_data, start, len);
    }

    // Get bytes, starting from LSB (right in notation)
    u8 sub_lsb(size_t start, size_t len) const
    {
        return sub_lsb(m_data, start, len);
    }

    bool operator[](size_t index) const
    {
        return m_data & (0x80 >> index);
    }

    u8 data() const { return m_data; }

private:
    u8 m_data;
};
