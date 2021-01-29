#include "Cx16.h"

Device* Bus::find_device(u8 id)
{
    auto it = m_devices.find(id);
    if(it == m_devices.end())
        return nullptr;
    return it->second.get();
}

void Bus::register_device(u8 id, std::shared_ptr<Device> device)
{
    std::cerr << name() << ": Registering device ID " << (int)id << ": " << device->name() << std::endl;
    device->set_master_bus(this);
    m_devices.insert(std::make_pair(id, device));
}

void Cx16Bus::out8(u8 id, u8 val)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        dev->out8(val);
    else
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
            {
                sw->out8(id, val);
                return;
            }
        }
        std::cerr << name() << ": Device ID not found: " << (int)id << std::endl;
    }
}

void Cx16Bus::out16(u8 id, u16 val)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        dev->out16(val);
    else
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
            {
                sw->out16(id, val);
                return;
            }
        }
        std::cerr << name() << ": Device ID not found: " << (int)id << std::endl;
    }
}

u16 Cx16Bus::in16(u8 id)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        return dev->in16();
    else
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                return sw->in16(id);
        }
        std::cerr << name() << ": Device ID not found: " << (int)id << std::endl;
    }
    return 0;
}

u8 Cx16Bus::in8(u8 id)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        return dev->in8();
    else
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                return sw->in8(id);
        }
        std::cerr << name() << ": Device ID not found: " << (int)id << std::endl;
    }
    return 0;
}

void Cx16Bus::register_io_switch(std::shared_ptr<Cx16Bus> bus)
{
    std::cerr << name() << ": Registering I/O Switch bus: " << bus->name() << std::endl;
    m_switches.push_back(bus);
}


void Cx16InterruptController::register_device(u8 irq, std::shared_ptr<Cx16Device> device)
{
    std::cerr << name() << ": Registering device on IRQ " << (int)irq << ": " << device->name() << std::endl;
    device->set_interrupt_controller(this);
    m_devices.insert(std::make_pair(irq, device));
}

void Cx16DMAController::register_device(u8 channel, std::shared_ptr<Cx16Device> device)
{
    std::cerr << name() << ": Registering device on DMA channel " << (int)channel << ": " << device->name() << std::endl;
    device->set_dma_controller(this);
    m_devices.insert(std::make_pair(channel, device));
}

u8 Memory::read_memory(u16 addr) const
{
    if(addr >= m_memory_size)
    {
        std::cerr << name() << ": read: Physical address out of range 0x" << std::hex << addr << " > 0x" << m_memory_size << std::dec << std::endl;
        return 0;
    }
    return m_memory[addr];
}
void Memory::write_memory(u16 addr, u8 val)
{
    if(addr >= m_memory_size)
    {
        std::cerr << name() << ": write: Physical address out of range 0x" << std::hex << addr << " > 0x" << m_memory_size << std::dec << std::endl;
        return;
    }
    m_memory[addr] = val;
}


void Cx16ConventionalDevice::out8(u8 val)
{
    switch(m_state)
    {
    case Ready:
        {
            switch(val)
            {
                case 0x00: m_state = RegisterReadRq; break;
                case 0x01: m_state = RegisterWriteRq; break;
                default:
                    m_args_needed = get_argc(val);
                    m_command = val;
                    m_state = CommandRq;
                    break;
            }
        } break;
    case RegisterReadRq:
        m_command = val;
        m_state = RegisterRead;
        break;
    case RegisterWriteRq:
        m_command = val;
        m_state = RegisterWrite;
        break;
    case CommandRq:
        out16(val);
        break;
    default:
        break;
    }
}
void Cx16ConventionalDevice::out16(u16 val)
{
    switch(m_state)
    {
    case CommandRq:
        m_arg_buf.push_back(val);
        if(!(--m_args_needed))
        {
            m_command_result = do_cmd(m_command, m_arg_buf);
            m_arg_buf.clear();
            m_state = Ready;
        }
        break;
    case RegisterWrite:
        {
            std::cerr << name() << ": Register write: regs[0x" << std::hex << (int)m_command << "] = 0x" << val << std::dec << std::endl;
            u16* _reg = reg(m_command);
            if(_reg)
                *_reg = val;
            m_state = Ready;
        } break;
    case RegisterRead:
        {
            std::cerr << name() << ": Register read: regs[0x" << std::hex << (int)m_command << "]" << std::dec << std::endl;
            u16* _reg = reg(m_command);
            if(_reg)
                m_command_result = *_reg;
            m_state = Ready;
        } break;
    default:
        break;
    }
}
u16 Cx16ConventionalDevice::in16()
{
    return m_command_result;
}
u8 Cx16ConventionalDevice::in8()
{
    // TODO: async commands!
    if(m_state == Irqc)
        return m_command;
    return 0;
}
bool Cx16ConventionalDevice::irq_raised() const
{
    // TODO: async commands!
    return false;
}
