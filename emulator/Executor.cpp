#include "ControlUnit.h"
#include "CPU.h"

void Executor::_INSN_OUT(bool width, u8 port, const Source& src)
{
    if(width)
        m_cpu.slave()->out8(port, src.read8());
    else
        m_cpu.slave()->out16(port, src.read());
}

void Executor::_INSN_IN(bool width, u8 port, Destination& dst)
{
    if(width)
        dst.write8(m_cpu.slave()->in8(port));
    else
        dst.write(m_cpu.slave()->in16(port));
}

void Executor::_INSN_MOV(Destination& dst, const Source& src)
{
    dst.write(src.read());
}

void Executor::_INSN_JMP(const Source& ia)
{
    m_ip.set_value(ia.read());
}

void Executor::jmp_helper(const Source& ioff)
{
    m_ip.set_value(m_ip.value() + ioff.read8());
}

void Executor::_INSN_JZ(const Source& ioff)
{
    if(m_flags & FLAG_ZERO)
        jmp_helper(ioff);
}

void Executor::_INSN_JG(const Source& ioff)
{
    if(m_flags & FLAG_GREATER)
        jmp_helper(ioff);
}

void Executor::_INSN_JL(const Source& ioff)
{
    if(m_flags & FLAG_OVERFLOW)
        jmp_helper(ioff);
}

void Executor::_INSN_JNE(const Source& ioff)
{
    if(!(m_flags & FLAG_ZERO))
        jmp_helper(ioff);
}

void Executor::_INSN_JGE(const Source& ioff)
{
    if(m_flags & FLAG_ZERO || m_flags & FLAG_GREATER)
        jmp_helper(ioff);
}

void Executor::_INSN_JLE(const Source& ioff)
{
    if(m_flags & FLAG_ZERO || m_flags & FLAG_OVERFLOW)
        jmp_helper(ioff);
}
