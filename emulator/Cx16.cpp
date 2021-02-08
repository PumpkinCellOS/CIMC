#include "Cx16.h"

#include <thread>

struct DeviceBootstrapper
{
    DeviceBootstrapper(Device& device)
    : m_device(device) {}

    void operator()()
    {
        debug("Bootstrapper") << "Booting device: " << m_device.name();
        m_device.boot();
        debug("Bootstrapper") << "Shutting down device: " << m_device.name();
    }

private:
    Device& m_device;
};

Device::Device()
: m_worker(DeviceBootstrapper(*this))
{
}

Device::~Device()
{
    m_device_destroyed.store(true);
    m_worker.join();
}

void Cx16InterruptController::boot()
{
    while(!m_device_destroyed.load())
    {
        std::lock_guard<std::mutex> lock(m_irq_access_mutex);
        m_irq_raised = false;
        for(auto it: m_devices)
        {
            Cx16Device& device = *it.second;
            if(device.irq_raised())
                m_irq_raised = true;
        }
    }
}

void Cx16Bus::out8(u8 id, u8 val)
{
    auto dev = (Cx16Device*)find_device(id);
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
        error(name()) << "Device ID not found: " << (int)id;
    }
}

void Cx16Bus::out16(u8 id, u16 val)
{
    auto dev = (Cx16Device*)find_device(id);
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
        error(name()) << "Device ID not found: " << (int)id;
    }
}

u16 Cx16Bus::in16(u8 id)
{
    auto dev = (Cx16Device*)find_device(id);
    if(dev)
        return dev->in16();
    else
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                return sw->in16(id);
        }
        error(name()) << "Device ID not found: " << (int)id;
    }
    return 0;
}

u8 Cx16Bus::in8(u8 id)
{
    auto dev = (Cx16Device*)find_device(id);
    if(dev)
        return dev->in8();
    else
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                return sw->in8(id);
        }
        error(name()) << "Device ID not found: " << (int)id;
    }
    return 0;
}

void Cx16Bus::register_io_switch(std::shared_ptr<Cx16Bus> bus)
{
    info(name()) << "Registering I/O Switch bus: " << bus->name();
    m_switches.push_back(bus);
}


void Cx16InterruptController::register_device(u8 irq, std::shared_ptr<Cx16Device> device)
{
    info(name()) << "Registering device on IRQ " << (int)irq << ": " << device->name();
    device->set_interrupt_controller(this);
    m_devices.insert(std::make_pair(irq, device));
}

void Cx16DMAController::register_device(u8 channel, std::shared_ptr<Cx16Device> device)
{
    info(name()) << "Registering device on DMA channel " << (int)channel << ": " << device->name();
    device->set_dma_controller(this);
    m_devices.insert(std::make_pair(channel, device));
}

u8 Memory::read_memory(u16 addr) const
{
    if(addr >= m_memory_size)
    {
        error(name()) << name() << "read: Physical address out of range 0x" << std::hex << addr << " > 0x" << m_memory_size << std::dec;
        return 0;
    }
    return m_memory[addr];
}
void Memory::write_memory(u16 addr, u8 val)
{
    if(addr >= m_memory_size)
    {
        error(name()) << name() << "write: Physical address out of range 0x" << std::hex << addr << " > 0x" << m_memory_size << std::dec;
        return;
    }
    m_memory[addr] = val;
}


void Cx16ConventionalDevice::out8(u8 val)
{
    trace(name()) << "out8:" << (int)val << " state=" << m_state << " args_left=" << (int)m_args_needed;
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
                    m_current_command.m_command = val;
                    m_state = CommandRq;
                    break;
            }
        } break;
    case RegisterReadRq:
        m_current_command.m_command = val;
        m_state = RegisterRead;
        break;
    case RegisterWriteRq:
        m_current_command.m_command = val;
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
    trace(name()) << "out16:" << (int)val << " state=" << m_state << " args_left=" << (int)m_args_needed;
    switch(m_state)
    {
    case CommandRq:
        m_current_command.m_arg_buf.push_back(val);
        {
            std::lock_guard<std::mutex> lock(m_queue_access_mutex);
            m_args_needed--;

            if(m_args_needed == 0)
            {
                m_pending_commands.push(m_current_command);
                m_current_command.m_arg_buf.clear();
                m_state = Ready;
            }
        }
        break;
    case RegisterWrite:
        {
            trace(name()) << "Register write: regs[0x" << std::hex << (int)m_current_command.m_command << "] = 0x" << val << std::dec;
            u16* _reg = reg(m_current_command.m_command);
            if(_reg)
                *_reg = val;
            m_state = Ready;
        } break;
    case RegisterRead:
        {
            trace(name()) << "Register read: regs[0x" << std::hex << (int)m_current_command.m_command << "]" << std::dec;
            u16* _reg = reg(m_current_command.m_command);
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
    if(m_state == Irqc)
        return m_current_command.m_command;
    return 0;
}
bool Cx16ConventionalDevice::irq_raised() const
{
    return m_state == Irqc;
}

void Cx16ConventionalDevice::boot()
{
    while(!m_device_destroyed.load())
    {
        std::lock_guard<std::mutex> lock(m_queue_access_mutex);
        while(!m_pending_commands.empty())
        {
            Command& command = m_pending_commands.front();
            m_command_result = do_cmd(command.m_command, command.m_arg_buf);
            m_pending_commands.pop();
        }
    }
}
