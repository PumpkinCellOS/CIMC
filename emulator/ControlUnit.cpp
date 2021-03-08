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
    Cx16InterruptController* pic = m_cpu.slave()->interrupt_controller();
    if(pic->irq_raised() && !get_flag(FLAG_IN_IRQ) && !get_flag(FLAG_EXCEPTION))
    {
        u8 number = pic->number();
        raise_interrupt(number, false);
    }

    MemorySource opcode = read_insn();
    u8 opcode_read = opcode.read8();
    trace("CU") << "cycle: 0x" << std::hex << (int)opcode_read << " (" << to_binary_string(opcode_read) << ")" << std::dec;
    do_insn(Bitfield(opcode_read));
}

void ControlUnit::raise_interrupt(u16 int_number, bool internal, u16* arg)
{
    debug("CU") << "Trying to raise " << (internal ? "internal" : "external") << " interrupt: " << int_number;

    // Wake up processor
    m_is_halted = false;

    bool ef = get_flag(FLAG_EXCEPTION);
    bool iif = get_flag(FLAG_IN_IRQ);

    // Double fault check
    if(internal && ef)
    {
        error("CU") << "Raised an " << (internal ? "internal" : "external") << " interrupt " << int_number << " while already in interrupt!";
        double_fault();
    }

    // Set flags now (it CANNOT be ignored)
    if(internal)
        set_flag(FLAG_EXCEPTION);

    // IVT check
    if(int_number >= m_ivt.size())
    {
        if(internal)
        {
            error("CU") << "ISR number out of bounds: " << int_number;
            u16 arg = UE_REALLY << 8;
            raise_interrupt(INT_UNSPECIFIED, true, &arg);
        }
        else
            info("CU") << "IRQ number out of bounds: " << int_number << ", ignoring";
        return;
    }

    // Here everything should work properly. If something faults when pushing values
    // on stack, we should try to raise a next interrupt.
    debug("CU") << (internal ? "Internal" : "External") << " Interrupt Raised! " << int_number;

    if(!internal)
        set_flag(FLAG_IN_IRQ);

    // Setup stack
    m_executor._INSN_PUSH(false, m_ip.as_source());
    m_executor._INSN_PUSH(false, ImmediateSource(m_flags));
    if(arg)
        m_executor._INSN_PUSH(false, ImmediateSource(*arg));

    // Jump to specified location basing on IVT.
    m_executor._INSN_JMP(ImmediateSource(m_ivt[int_number]));
}

void ControlUnit::return_from_interrupt()
{
    bool ef = get_flag(FLAG_EXCEPTION);
    bool iif = get_flag(FLAG_IN_IRQ);

    // Clear flags or raise an exception
    if(ef)
        clear_flag(FLAG_EXCEPTION);
    else if(iif)
        clear_flag(FLAG_IN_IRQ);
    else
    {
        error("CU") << "Invalid IRET: Interrupt Underflow";
        u16 arg = UE_INVALID_IRET << 8;
        raise_interrupt(INT_UNSPECIFIED, true, &arg);
    }

    // Pop stack and return to previous location.
    ImmediateDestination destination(m_flags);
    m_executor._INSN_POP(false, destination);
    m_executor._INSN_POP(false, m_ip.as_destination());
}

MemorySource ControlUnit::read_insn()
{
    MemorySource source = virtual_memory_readable(m_ip.value());
    m_ip.set_value(m_ip.value() + 1);
    //trace("CU") << "read_insn at " << m_ip.value() << ": 0x" << std::hex << (int)source.read8() << std::dec;
    return source;
}

MemorySource ControlUnit::read_2_insns()
{
    // NOTE: The value is automatically 16-bit!
    MemorySource source = virtual_memory_readable(m_ip.value());
    m_ip.set_value(m_ip.value() + 2);
    //trace("CU") << "read_2_insns at " << m_ip.value() << ": 0x" << std::hex << (int)source.read() << std::dec;
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
    debug("CU") << "---- GPR dump ----" << std::hex << std::uppercase << std::setfill('0');
    debug("CU") << "AX=" << std::setw(4) << m_ax.value() << " BX=" << std::setw(4) << m_bx.value();
    debug("CU") << "CX=" << std::setw(4) << m_cx.value() << " DX=" << std::setw(4) << m_dx.value();
    debug("CU") << "SP=" << std::setw(4) << m_sp.value() << " BP=" << std::setw(4) << m_bp.value();
    debug("CU") << "IP=" << std::setw(4) << m_ip.value() << " FL=" << std::setw(4) << m_flags;
    debug("CU") <<  std::nouppercase << std::dec;
}

void ControlUnit::cpuid()
{
    // Revision: none
    // ALU: present
    // Major Version: cx16 (0x4)
    // Minor Version: Initial Release (0x0)
    m_ax.set_value(0x0840);
}

void ControlUnit::do_insn(Bitfield opcode)
{
    //trace("CU") << "Opcode: " << to_binary_string(opcode.data());
    //trace("CU") << "Sub: " << to_binary_string(opcode.sub_msb(0, 3));

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
        trace("CU") << "Jump: type=" << to_binary_string(type);
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
    // Pop
    else if(opcode.sub_msb(0, 5) == 0b10001)
    {
        bool pop_all = opcode[5];
        bool spop = opcode[6];
        bool reserved = opcode[7];

        // TODO: Remove Stack Frame
        if(pop_all)
        {
            error("CU") << "Stack Special not implemented: " << std::hex << (int)opcode.data() << std::dec;
            raise_interrupt(INT_INVALID_INSTRUCTION, true);
            return;
        }
        else
        {
            m_executor._INSN_POP(spop, m_ax.as_destination());
        }
    }
    // Add
    else if(opcode.sub_msb(0, 5) == 0b10010)
    {
        bool dst = opcode[5];
        u8 src = opcode.sub_msb(6, 2);

        Data data = (dst ? (src == 0b11 ? m_bx : m_dx) : m_ax).as_data();
        m_executor._INSN_ADD(data,
                             src == 0b00 ? (const Source&)read_2_insns() : (const Source&)common_source(src));
    }
    // Subtract
    else if(opcode.sub_msb(0, 5) == 0b10011)
    {
        bool dst = opcode[5];
        u8 src = opcode.sub_msb(6, 2);

        Data data = (dst ? (src == 0b11 ? m_bx : m_dx) : (src == 0b11 ? m_sp : m_ax)).as_data();
        m_executor._INSN_SUB(data,
                             src == 0b00 || (src == 0b11 && !dst) ? (const Source&)read_2_insns() : (const Source&)common_source(src));
    }
    // TODO.... Multiply/Divide, And, Or, Other Byte, Call
    else if(opcode.sub_msb(0, 6) == 0b101101)
    {
        bool li = opcode[6];
        bool op = opcode[7];
        if(li)
            m_executor._INSN_LIVT(op ? m_ax.as_source() : (const Source&)read_2_insns());
        else
            m_executor._INSN_INT(op ? INT_USER1 : INT_USER2);
    }
    // TODO: Random Block 2
    // Random Block 2
    else if(opcode.sub_msb(0, 6) == 0b101111)
    {
        u8 op = opcode.sub_msb(6, 2);
        switch(op)
        {
            case 0b00: m_executor._INSN_HLT(); break;
            case 0b10: m_executor._INSN_CPUID(); break;
            default:
                error("CU") << "RB2 not implemented: " << std::hex << (int)opcode.data() << std::dec;
                raise_interrupt(INT_INVALID_INSTRUCTION, true);
                break;
        }
    }
    // TODO: XOR, Memory & Stack Store, dx Store Special, Exchange, Extended, Store Stack
    else
    {
        error("CU") << "Invalid opcode: " << std::hex << (int)opcode.data() << std::dec;
        raise_interrupt(INT_INVALID_INSTRUCTION, true);
    }
}
