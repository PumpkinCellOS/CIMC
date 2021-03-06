#pragma once

#include "Config.h"
#include "Cx16.h"

class CPU;

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

    RegSrc as_source_with_offset(int off) const { return RegSrc(*this, off); }

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

class ControlUnit;

class Executor
{
public:
    Executor(ControlUnit& control)
    : m_control(control) {}

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
    void _INSN_CMP(const Source& op1, const Source& op2);
    void _INSN_PUSH(bool width, const Source& value);

private:
    void jmp_helper(const Source& ioff);

    ControlUnit& m_control;
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

#define INT_INVALID_MATH 0
#define INT_INVALID_INSTRUCTION 1
#define INT_UNSPECIFIED 2
#define INT_PAGE_FAULT 3
#define INT_USER1 6
#define INT_USER2 7

#define UE_REALLY 0x0
#define UE_INVALID_IRET 0x1
#define UE_NMI 0x2

class ControlUnit
{
public:
    ControlUnit(CPU& cpu)
    : m_cpu(cpu), m_executor(*this) {}

    MemorySource read_insn();
    MemorySource read_2_insns();
    void do_insn(Bitfield opcode);
    void cycle();

    MemorySource virtual_memory_readable(u16 addr);
    MemoryDestination virtual_memory_writable(u16 addr);

    Register& instruction_pointer() { return m_ip; }
    Register& stack_pointer() { return m_sp; }
    Register& base_pointer() { return m_bp; }

    u16 flags() const { return m_flags; }
    void set_flag(u16 flag) { m_flags |= flag; }

    // TODO: Find out a better way to set flags!
    void set_flag_to(u16 flag, bool data) { if(data) set_flag(flag); else clear_flag(flag); }

    void clear_flag(u16 flag) { m_flags &= ~flag; }
    bool get_flag(u16 flag) const { return m_flags & flag; }

    CPU& cpu() { return m_cpu; }

    void dump_registers();

    void raise_interrupt(u16 int_number, bool internal, u16* arg = nullptr);
    void return_from_interrupt();

private:
    void double_fault() { error("CU") << "Double fault :("; exit(-1); }

    CPU& m_cpu;
    Executor m_executor;
    Register m_ax;
    Register m_bx;
    Register m_cx;
    Register m_dx;

    Register m_ip;
    Register m_sp;
    Register m_bp;

    u16 m_flags = 0;
    std::vector<u16> m_ivt;

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

