#pragma once

#include "Config.h"
#include "Cx16.h"

class CPU;

class Bitfield
{
public:
    Bitfield(u16 data)
    : m_data(data) {}

    u16 sub(size_t start, size_t len) const
    {
        // TODO: Find out a better way to generate a mask!
        u16 mask = 0xFFFF;
        mask <<= start;
        mask >>= start;
        u8 b = (16-start-len);
        mask >>= b;
        mask <<= b;
        return (m_data & mask) << start;
    }

    bool operator[](size_t index) const
    {
        return m_data & (1 << index);
    }

    u16 data() const { return m_data; }

private:
    u16 m_data;
};

class Destination
{
public:
    virtual void write(u16 data) = 0;
    virtual void write8(u8 data) { return write(data); }
};

class Source
{
public:
    virtual u16 read() const = 0;
    virtual u8 read8() const { return read() & 0xFF; }
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

class Register
{
public:
    explicit Register(u16 value = 0)
    : m_value(value) {}

    class RegSrc : public Source
    {
    public:
        RegSrc(const Register& reg)
        : m_register(reg) {}

        virtual u16 read() const override { return m_register.value(); }

    private:
        const Register& m_register;
    };

    class RegDst : public Destination
    {
    public:
        RegDst(Register& reg)
        : m_register(reg) {}

        virtual void write(u16 value) override { m_register.set_value(value); }

    private:
        Register& m_register;
    };

    const Source& as_source() const { return m_source; }
    Destination& as_destination() { return m_destination; }

    u16 value() const { return m_value; }
    void set_value(u16 value) { m_value = value; }

private:
    u16 m_value;
    RegSrc m_source { *this };
    RegDst m_destination { *this };
};

#define FLAG_INTERRUPT 0x01
#define FLAG_GREATER   0x02
#define FLAG_OVERFLOW  0x04
#define FLAG_ZERO      0x08
#define FLAG_IN_IRQ    0x10
#define FLAG_PAGING    0x20
#define FLAG_EXCEPTION 0x40

class Executor
{
public:
    Executor(CPU& cpu)
    : m_cpu(cpu) {}

    // Instructions CANNOT see registers directly (they don't know their operands).
    void _INSN_OUT(bool width, u8 port, const Source& src);
    void _INSN_IN(bool width, u8 port, Destination& dst);
    void _INSN_MOV(Destination& dst, const Source& src);
    void _INSN_JMP(const Source& ia);
    void _INSN_JZ(const Source& ioff);
    void _INSN_JG(const Source& ioff);
    void _INSN_JL(const Source& ioff);
    void _INSN_JNE(const Source& ioff);
    void _INSN_JGE(const Source& ioff);
    void _INSN_JLE(const Source& ioff);

    Register& instruction_pointer() { return m_ip; }

private:
    void jmp_helper(const Source& ioff);

    CPU& m_cpu;
    u16 m_flags = 0;
    Register m_ip;
};

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
    ImmediateDestination(u16 value)
    : m_value(value) {}

    virtual void write(u16 value) override { m_value = value; }

private:
    u16 m_value;
};


class ControlUnit
{
public:
    ControlUnit(CPU& cpu)
    : m_cpu(cpu), m_executor(cpu) {}

    MemorySource read_insn();
    MemorySource read_2_insns();
    void do_insn(Bitfield opcode);
    void cycle();

    MemorySource virtual_memory_readable(u16 addr);
    MemoryDestination virtual_memory_writable(u16 addr);

private:
    CPU& m_cpu;
    Executor m_executor;
    Register m_ax;
    Register m_bx;
    Register m_cx;
    Register m_dx;
    Register m_sp;
    Register m_bp;

    class ErrorSource : public Source
    {
    public:
        virtual u16 read() const override { error("CU") << "Invalid Read from ErrorSource"; exit(-1); }
    };

    class ErrorDestination : public Destination
    {
    public:
        virtual void write(u16) override { error("CU") << "Invalid Write to ErrorSource"; exit(-1); }
    };

    ErrorSource m_error_source;
    ErrorDestination m_error_destination;

    const Source& common_source(u8 type) const;
    Destination& common_destination(u8 type);
};

