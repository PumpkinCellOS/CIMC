#pragma once

#include "Config.h"
#include "Data.h"

class Register
{
public:
    explicit Register(u16 value = 0)
    : m_value(value) {}

    class RegSrc : public Source
    {
    public:
        RegSrc(const Register& reg, int offset = 0)
        : m_register(reg), m_offset(offset) {}

        virtual u16 read() const override { return m_register.value() + m_offset; }

    private:
        const Register& m_register;
        int m_offset;
    };

    class RegDst : public Destination
    {
    public:
        RegDst(Register& reg)
        : m_register(reg) {}

        virtual void write(u16 value) override { m_register.set_value(value); }

    private:
        Register& m_register;
        int m_offset;
    };

    const Source& as_source() const { return m_source; }
    Destination& as_destination() { return m_destination; }
    Data as_data() { return Data(m_source, m_destination); }

    RegSrc as_source_with_offset(int off) const { return RegSrc(*this, off); }

    u16 value() const { return m_value; }
    void set_value(u16 value) { m_value = value; }

private:
    u16 m_value;
    RegSrc m_source { *this };
    RegDst m_destination { *this };
};
