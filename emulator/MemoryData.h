#pragma once

#include "Data.h"
// Memory

class MemorySource : public Source
{
public:
    MemorySource(const MemoryAccessNode& access, u16 address)
    : m_access(access), m_address(address) {}

    // NOTE: It defines byte order!
    virtual u16 read() const override
        { return (m_access.read_memory(m_address) << 8) | m_access.read_memory(m_address + 1); }

    virtual u8 read8() const override
        { return m_access.read_memory(m_address); }

private:
    const MemoryAccessNode& m_access;
    u16 m_address;
};

class MemoryDestination : public Destination
{
public:
    MemoryDestination(MemoryAccessNode& access, u16 address)
    : m_access(access), m_address(address) {}

    virtual void write(u16 value) override
        { m_access.write_memory(m_address, value & 0xFF);
          m_access.write_memory(m_address + 1, (value & 0xFF00) >> 8); }

    virtual void write8(u8 value) override
        { return m_access.write_memory(m_address, value & 0xFF); }

private:
    MemoryAccessNode& m_access;
    u16 m_address;
};
