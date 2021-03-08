#pragma once

#include "Data.h"

// Immediate
class ImmediateSource : public Source
{
public:
    ImmediateSource(u16 value)
    : m_value(value) {}

    virtual u16 read() const override { return m_value; }

private:
    u16 m_value;
};

class ImmediateDestination : public Destination
{
public:
    ImmediateDestination(u16& value)
    : m_value(value) {}

    virtual void write(u16 value) override { m_value = value; }

private:
    u16& m_value;
};
