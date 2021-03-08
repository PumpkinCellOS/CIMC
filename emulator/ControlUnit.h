#pragma once

#include "Bitfield.h"
#include "Config.h"
#include "CPUConstants.h"
#include "Cx16.h"
#include "Data.h"
#include "Executor.h"
#include "ImmediateData.h"
#include "MemoryData.h"
#include "Register.h"

class CPU;

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

    bool is_halted() const { return m_is_halted; }
    void halt() { info("CU") << "Halt!"; m_is_halted = true; }
    void wake() { info("CU") << "Wake!"; m_is_halted = false; }

    void set_ivt_size(u8 sz) { m_ivt.resize(sz); }
    void set_isr(u8 int_number, u16 address) { m_ivt[int_number] = address; }

    void cpuid();

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

    bool m_is_halted = false;

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

