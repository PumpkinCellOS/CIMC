#include "ControlUnit.h"
#include "CPU.h"

void Executor::_INSN_OUT(bool width, u8 port, const Source& src)
{
    if(width)
    {
        trace("eXecutor") << "I/O: out8 0x" << std::hex << (int)port << ", 0x" << (int)src.read8() << std::dec;
        m_control.cpu().slave()->out8(port, src.read8());
    }
    else
    {
        trace("eXecutor") << "I/O: out16 0x" << std::hex << (int)port << ", 0x" << src.read() << std::dec;
        m_control.cpu().slave()->out16(port, src.read());
    }
}

void Executor::_INSN_IN(bool width, u8 port, Destination& dst)
{
    if(width)
    {
        trace("eXecutor") << "I/O: in8 0x" << std::hex << (int)port << std::dec;
        dst.write8(m_control.cpu().slave()->in8(port));
    }
    else
    {
        trace("eXecutor") << "I/O: in16 0x" << std::hex << (int)port << std::dec;
        dst.write(m_control.cpu().slave()->in16(port));
    }
}

void Executor::_INSN_MOV(Destination& dst, const Source& src)
{
    trace("eXecutor") << "I/O: mov " << src.read();
    dst.write(src.read());
}

void Executor::_INSN_JMP(const Source& ia)
{
    m_control.instruction_pointer().set_value(ia.read());
}

void Executor::jmp_helper(const Source& ioff)
{
    m_control.instruction_pointer().set_value(m_control.instruction_pointer().value() + ioff.read8());
}

void Executor::_INSN_JZ(const Source& ioff)
{
    if(m_control.get_flag(FLAG_ZERO))
        jmp_helper(ioff);
}

void Executor::_INSN_JG(const Source& ioff)
{
    if(m_control.get_flag(FLAG_GREATER))
        jmp_helper(ioff);
}

void Executor::_INSN_JL(const Source& ioff)
{
    if(m_control.get_flag(FLAG_OVERFLOW))
        jmp_helper(ioff);
}

void Executor::_INSN_JNE(const Source& ioff)
{
    if(!m_control.get_flag(FLAG_ZERO))
        jmp_helper(ioff);
}

void Executor::_INSN_JGE(const Source& ioff)
{
    if(m_control.get_flag(FLAG_ZERO) || m_control.get_flag(FLAG_GREATER))
        jmp_helper(ioff);
}

void Executor::_INSN_JLE(const Source& ioff)
{
    if(m_control.get_flag(FLAG_ZERO) || m_control.get_flag(FLAG_OVERFLOW))
        jmp_helper(ioff);
}

void Executor::_INSN_CMP(const Source& op1, const Source& op2)
{
    m_control.set_flag_to(FLAG_ZERO, (op1.read() == op2.read()));
    m_control.set_flag_to(FLAG_OVERFLOW, (op1.read() < op2.read()));
    m_control.set_flag_to(FLAG_GREATER, (op1.read() > op2.read()));
}

void Executor::_INSN_PUSH(bool width, const Source& value)
{
    auto& stack_pointer = m_control.stack_pointer();
    auto stack_top = m_control.virtual_memory_writable(stack_pointer.value());
    if(width)
    {
        stack_top.write8(value.read8());
        stack_pointer.set_value(stack_pointer.value() + 1);
    }
    else
    {
        stack_top.write(value.read());
        stack_pointer.set_value(stack_pointer.value() + 2);
    }
}

void Executor::_INSN_POP(bool width, Destination& value)
{
    auto& stack_pointer = m_control.stack_pointer();
    if(width)
    {
        stack_pointer.set_value(stack_pointer.value() - 1);
        auto stack_top = m_control.virtual_memory_readable(stack_pointer.value());
        value.write8(stack_top.read8());
    }
    else
    {
        stack_pointer.set_value(stack_pointer.value() - 2);
        auto stack_top = m_control.virtual_memory_readable(stack_pointer.value());
        value.write(stack_top.read());
    }
}

void Executor::_INSN_LIVT(const Source& addr)
{
    u16 addr_num = addr.read();
    u8 size = m_control.virtual_memory_readable(addr_num).read8();
    m_control.set_ivt_size(size + 1);
    addr_num++;
    for(size_t s = 0; s <= size; s++)
    {
        m_control.set_isr(s, m_control.virtual_memory_readable(addr_num).read());
        addr_num += 2;
    }
}

void Executor::_INSN_INT(u8 int_number)
{
    m_control.raise_interrupt(int_number, true);
}

void Executor::_INSN_HLT()
{
    m_control.halt();
}

void Executor::_INSN_CPUID()
{
    m_control.cpuid();
}
