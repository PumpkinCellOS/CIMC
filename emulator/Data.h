#pragma once

#include "Config.h"

class Destination
{
public:
    virtual void write(u16 data) = 0;
    virtual void write8(u8 data) { return write(data); }

    virtual bool can_write() const { return true; }
};

class Source
{
public:
    virtual u16 read() const = 0;
    virtual u8 read8() const { return read() & 0xFF; }

    virtual bool can_read() const { return true; }
};

class Data
{
public:
    Data(const Source& src, Destination& dst)
    : m_src(src), m_dst(dst) {}

    const Source& source() const { return m_src; }
    Destination& destination() { return m_dst; }

private:
    const Source& m_src;
    Destination& m_dst;
};
