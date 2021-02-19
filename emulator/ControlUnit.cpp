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
    u8 opcode_read = opcode.read8();
    trace("CU") << "cycle: 0x" << std::hex << (int)opcode_read << std::dec;
    do_insn(Bitfield(opcode_read));
}

MemorySource ControlUnit::read_insn()
{
    MemorySource source = virtual_memory_readable(m_ip.value());
    m_ip.set_value(m_ip.value() + 1);
    trace("CU") << "read_insn at " << m_ip.value() << ": 0x" << std::hex << (int)source.read8() << std::dec;
    return source;
}

MemorySource ControlUnit::read_2_insns()
{
    // NOTE: The value is automatically 16-bit!
    MemorySource source = virtual_memory_readable(m_ip.value());
    m_ip.set_value(m_ip.value() + 2);
    trace("CU") << "read_2_insns at " << m_ip.value() << ": 0x" << std::hex << (int)source.read() << std::dec;
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

void ControlUnit::dump_registers()
{
    debug("CU") << "---- Register dump ----" << std::hex << std::uppercase << std::setfill('0');
    debug("CU") << "AX=" << std::setw(4) << m_ax.value() << " BX=" << std::setw(4) << m_bx.value();
    debug("CU") << "CX=" << std::setw(4) << m_cx.value() << " DX=" << std::setw(4) << m_dx.value();
    debug("CU") << "SP=" << std::setw(4) << m_sp.value() << " BP=" << std::setw(4) << m_bp.value();
    debug("CU") << "IP=" << std::setw(4) << m_ip.value() << std::nouppercase << std::dec;
}

void ControlUnit::do_insn(Bitfield opcode)
{
    trace("CU") << "Opcode: " << to_binary_string(opcode.data());
    trace("CU") << "Sub: " << to_binary_string(opcode.sub_msb(0, 3));

    // I/O Operations
    if(opcode.sub_msb(0, 2) == 0b00)
    {
        bool io = opcode[2];
        bool width = opcode[3];
        u8 port = opcode.sub_msb(4, 4);
        if(io)
            m_executor._INSN_IN(width, port, m_ax.as_destination());
        else
            m_executor._INSN_OUT(width, port, m_ax.as_source());
    }
    // I/O Output (Immediate)
    else if(opcode.sub_msb(0, 3) == 0b010)
    {
        bool width = opcode[3];
        u8 port = opcode.sub_msb(4, 4);
        m_executor._INSN_OUT(width, port, width ? read_insn() : read_2_insns());
    }
    // Store
    else if(opcode.sub_msb(0, 4) == 0b0110)
    {
        u8 dst = opcode.sub_msb(4, 2);
        u8 src = opcode.sub_msb(6, 2);
        trace("CU") << "Store: dst=" << (int)dst << " src=" << (int)src;
        if(dst == src)
            m_executor._INSN_MOV(common_destination(dst), read_2_insns());
        else
            m_executor._INSN_MOV(common_destination(dst), common_source(src));
    }
    // Jump
    else if(opcode.sub_msb(0, 5) == 0b01110)
    {
        u8 type = opcode.sub_msb(5, 3);
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
    // Compare
    else if(opcode.sub_msb(0, 5) == 0b01111)
    {
        u8 op1 = opcode.sub_msb(5, 2);
        bool op2 = opcode[7];

        if(op1 == 0b11 && op2 == false)
            exit(-1);

        auto& reg = common_source(op1);
        if(op2)
            m_executor._INSN_CMP(reg, m_dx.as_source());
        else
            m_executor._INSN_CMP(reg, read_2_insns());
    }
    // Push
    else if(opcode.sub_msb(0, 5) == 0b10000)
    {
        bool width = opcode[5];
        u8 operand = opcode.sub_msb(6, 2);
        switch(operand)
        {
            case 0b00: m_executor._INSN_PUSH(width, m_ax.as_source()); break;
            case 0b01: m_executor._INSN_PUSH(width, width ? read_insn() : read_2_insns()); break;
            case 0b10: m_executor._INSN_PUSH(width, m_bp.as_source_with_offset((i8)(read_insn().read8()))); break;
            case 0b11: m_executor._INSN_PUSH(width, m_sp.as_source_with_offset((i8)(read_insn().read8()))); break;
        }
    }
    // TODO....
    else
    {
        error("CU") << "Invalid opcode: " << std::hex << (int)opcode.data() << std::dec;
        exit(-1);
    }
}
