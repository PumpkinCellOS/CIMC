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
    std::cerr << "Bus: Registering device ID " << (int)id << ": " << device->name() << std::endl;
    device->set_bus(this);
    m_devices.insert(std::make_pair(id, device));
}

void Cx16Bus::out8(u8 id, u8 val)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        dev->out8(val);
    else
        std::cerr << "Bus: Device ID not found: " << (int)id << std::endl;
}

void Cx16Bus::out16(u8 id, u16 val)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        dev->out16(val);
    else
        std::cerr << "Bus: Device ID not found: " << (int)id << std::endl;
}

u16 Cx16Bus::in16(u8 id)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        return dev->in16();
    else
        std::cerr << "Bus: Device ID not found: " << (int)id << std::endl;
    return 0;
}

u8 Cx16Bus::in8(u8 id)
{
    auto dev = dynamic_cast<Cx16Device*>(find_device(id));
    if(dev)
        return dev->in8();
    else
        std::cerr << "Bus: Device ID not found: " << (int)id << std::endl;
    return 0;
}


void Cx16InterruptController::register_device(u8 irq, std::shared_ptr<Cx16IRQCapableDevice> device)
{
    std::cerr << "Cx16InterruptController: Registering device on IRQ " << (int)irq << ": " << device->name() << std::endl;
    device->set_interrupt_controller(this);
    m_devices.insert(std::make_pair(irq, device));
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
                    std::cerr << (int)m_args_needed << " " << (int)m_command << " " << (int)m_state << std::endl;
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
            std::cerr << "Cx16Device command: " << (int)m_command << std::endl;
            m_command_result = do_cmd(m_command, m_arg_buf);
            m_arg_buf.clear();
            m_state = Ready;
        }
        break;
    case RegisterWrite:
        {
            u16* _reg = reg(m_command);
            if(_reg)
                *_reg = val;
            m_state = Ready;
        } break;
    case RegisterRead:
        {
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
