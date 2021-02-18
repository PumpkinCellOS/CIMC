#include "ControlUnit.h"

#include "CPU.h"

const Source& ControlUnit::common_source(u8 type) const
{
    switch(type)
    {
        case 0b00: return m_ax.as_source();
        case 0b01: return m_bx.as_source();
        case 0b10: return m_cx.as_source();
        case 0b11: return m_dx.as_source();
        default: return m_error_source;
    }
}

Destination& ControlUnit::common_destination(u8 type)
{
    switch(type)
    {
        case 0b00: return m_ax.as_destination();
        case 0b01: return m_bx.as_destination();
        case 0b10: return m_cx.as_destination();
        case 0b11: return m_dx.as_destination();
        default: return m_error_destination;
    }
}

void ControlUnit::cycle()
{
    // TODO: Check interrupts!

    MemorySource opcode = read_insn();
    trace("CU") << "cycle: 0x" << std::hex << (int)opcode.read8() << std::dec;
    do_insn(Bitfield(opcode.read8()));
}

MemorySource ControlUnit::read_insn()
{
    auto& ip = m_executor.instruction_pointer();
    MemorySource source = virtual_memory_readable(ip.value());
    ip.set_value(ip.value() + 1);
    trace("CU") << "read_insn at " << ip.value() << ": 0x" << std::hex << (int)source.read8() << std::dec;
    return source;
}

MemorySource ControlUnit::read_2_insns()
{
    // NOTE: The value is automatically 16-bit!
    auto& ip = m_executor.instruction_pointer();
    MemorySource source = virtual_memory_readable(ip.value());
    ip.set_value(ip.value() + 2);
    trace("CU") << "read_2_insns at " << ip.value() << ": 0x" << std::hex << (int)source.read() << std::dec;
    return source;
}

MemorySource ControlUnit::virtual_memory_readable(u16 addr)
{
    auto& node = addr < 384 ? (MemoryAccessNode&)m_cpu.rom() : (MemoryAccessNode&)*m_cpu.slave()->dma_controller();
    return MemorySource(node, m_cpu.map_virtual_to_physical(addr));
}

MemoryDestination ControlUnit::virtual_memory_writable(u16 addr)
{
    auto& node = addr < 384 ? (MemoryAccessNode&)m_cpu.rom() : (MemoryAccessNode&)*m_cpu.slave()->dma_controller();
    return MemoryDestination(node, m_cpu.map_virtual_to_physical(addr));
}

void ControlUnit::do_insn(Bitfield opcode)
{
    // I/O Operations
    if(opcode.sub(0, 2) == 0b00)
    {
        bool io = opcode[2];
        bool width = opcode[3];
        u8 port = opcode.sub(4, 4);
        trace("CU") << "I/O: io=" << io << " width=" << width << " port=" << (int)port;
        if(io)
            m_executor._INSN_IN(width, port, m_ax.as_destination());
        else
            m_executor._INSN_OUT(width, port, m_ax.as_source());
    }
    // I/O Output (Immediate)
    else if(opcode.sub(0, 3) == 0b010)
    {
        bool width = opcode[3];
        u8 port = opcode.sub(4, 4);
        m_executor._INSN_OUT(width, port, read_insn());
    }
    // Store
    else if(opcode.sub(0, 4) == 0b0110)
    {
        u8 dst = opcode.sub(4, 2);
        u8 src = opcode.sub(6, 2);
        if(dst == src)
            m_executor._INSN_MOV(common_destination(dst), read_2_insns());
        else
            m_executor._INSN_MOV(common_destination(dst), common_source(src));
    }
    // Jump
    else if(opcode.sub(0, 5) == 0b01110)
    {
        u8 type = opcode.sub(5, 3);
        switch(type)
        {
            case 0x0: m_executor._INSN_JMP(m_ax.as_source()); break;
            case 0x1: m_executor._INSN_JMP(read_2_insns()); break;
            case 0x2: m_executor._INSN_JZ(read_insn()); break;
            case 0x3: m_executor._INSN_JG(read_insn()); break;
            case 0x4: m_executor._INSN_JL(read_insn()); break;
            case 0x5: m_executor._INSN_JNE(read_insn()); break;
            case 0x6: m_executor._INSN_JGE(read_insn()); break;
            case 0x7: m_executor._INSN_JLE(read_insn()); break;
        }
    }
    else
    {
        exit(-1);
    }
    /*
    // Compare
    else if(opcode.sub(0, 5) == 0b01111)
    {
        u8 op1 = opcode.sub(5, 2);
        bool op2 = opcode[7];
        // TODO
    }*/
    // TODO....
}
