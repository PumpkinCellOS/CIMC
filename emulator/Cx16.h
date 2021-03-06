#pragma once

#include "Config.h"

class Bus;

class Device
{
public:
    static std::vector<Device*> all_devices;

    Device();
    virtual ~Device();

    virtual std::string name() const { return "Generic Device"; }

    void set_master_bus(Bus* bus) { m_bus = bus; }

    virtual void boot() { error(name()) << "Dummy boot called!"; }
    void power_on();

    void shutdown() { m_device_destroyed.store(true); }
    bool running() const { return m_device_destroyed.load(); }

protected:
    Bus* m_bus = nullptr;
    std::thread m_worker;
    std::atomic_bool m_device_destroyed;
};

class Bus : public Device
{
public:
    void register_device(u8 id, std::shared_ptr<Device> device)
    {
        std::lock_guard<std::mutex> lock(m_access_mutex);
        info(name()) << "Registering device ID " << (int)id << ": " << device->name();
        device->set_master_bus(this);
        m_devices.insert(std::make_pair(id, device));
    }

    virtual bool has_device(u8 id) { return find_device(id); }
    virtual std::string name() const override { return "Generic Bus"; }

protected:
    Device* find_device(u8 id)
    {
        std::lock_guard<std::mutex> lock(m_access_mutex);
        auto it = m_devices.find(id);
        if(it == m_devices.end())
            return nullptr;
        return it->second.get();
    }

    // Func(Device&)
    template<class Func>
    void for_each_device(Func&& func)
    {
        std::lock_guard<std::mutex> lock(m_access_mutex);
        for(auto dev: m_devices)
            func(*dev.second.get());
    }

private:
    std::mutex m_access_mutex;
    std::map<u8, std::shared_ptr<Device>> m_devices;
};

class Cx16Device;

class MemoryAccessNode
{
public:
    void set_master(MemoryAccessNode* node) { m_master_node = node; }

    virtual u8 read_memory(u16 addr) const = 0;
    virtual void write_memory(u16 addr, u8 val) = 0;

protected:
    MemoryAccessNode* m_master_node;
};

class PMIController;

class PMICapableDevice
{
public:
    virtual void pmi_boot() { debug("PMI") << "PMI boot signal sent"; }
    virtual void pmi_reboot() { debug("PMI") << "PMI reboot signal sent"; }
    virtual void pmi_shutdown() { debug("PMI") << "PMI shutdown signal sent"; }

    void set_pmi_controller(PMIController* pmic) { m_pmic = pmic; }

private:
    PMIController* m_pmic = nullptr;
};

// TODO: Allow specyfing image for memory
class Memory : public MemoryAccessNode, public PMICapableDevice, public Device
{
public:
    Memory(u16 size)
    : m_memory_size(size)
        { m_memory = std::make_unique<u8[]>(size); info(name()) << "Size: " << std::setprecision(2) << (size / 1024.0) << " kB"; }

    virtual u8 read_memory(u16 addr) const override;
    virtual void write_memory(u16 addr, u8 val) override;

    virtual void boot() override {}

    virtual std::string name() const override { return "Main Memory"; }

    size_t size() const { return m_memory_size; }

private:
    std::unique_ptr<u8[]> m_memory;
    size_t m_memory_size;
};

class Cx16DMAController : public Device, public MemoryAccessNode
{
public:
    void register_device(u8 channel, std::shared_ptr<Cx16Device> device);

    virtual std::string name() const override { return "Cx16 DMA Controller"; }

    virtual u8 read_memory(u16 addr) const override { return m_master_node->read_memory(addr); }
    virtual void write_memory(u16 addr, u8 val) override { m_master_node->write_memory(addr, val); }

    virtual void boot() override {}

private:
    std::map<u8, std::shared_ptr<Cx16Device>> m_devices;
};

class PMIController;
class Cx16InterruptController;

class Cx16Device : public Device
{
public:
    virtual void out8(u8 val) = 0;
    virtual void out16(u16 val) = 0;
    virtual u16 in16() = 0;
    virtual u8 in8() = 0;

    virtual u8 di_caps() const = 0;

    virtual std::string name() const override { return "Cx16 Generic Device"; }

    virtual bool irq_raised() const = 0;

    void set_interrupt_controller(Cx16InterruptController* irqc) { m_irqc = irqc; }
    void set_dma_controller(Cx16DMAController* dmac) { m_dmac = dmac; }

protected:
    Cx16InterruptController* m_irqc = nullptr;
    Cx16DMAController* m_dmac = nullptr;
};

class Cx16Bus;

class Cx16Master : public Device
{
public:
    virtual void set_slave(Cx16Bus* bus) { m_slave = bus; }

protected:
    Cx16Bus* m_slave = nullptr;
};

class Cx16Bus : public Bus
{
public:
    virtual void out8(u8 id, u8 val);
    virtual void out16(u8 id, u16 val);
    virtual u16 in16(u8 id);
    virtual u8 in8(u8 id);

    void register_io_switch(std::shared_ptr<Cx16Bus> bus);

    virtual void boot() override {}

    virtual std::string name() const override { return "Cx16 Bus"; }

private:
    std::vector<std::shared_ptr<Cx16Bus>> m_switches;
};

#define CX16_REG_READ  0x00
#define CX16_REG_WRITE 0x01

class Cx16ConventionalDevice : public Cx16Device, public PMICapableDevice
{
    enum State
    {
        Ready,
        RegisterWrite,
        CommandRq,
        Working,
        Irqc
    };
protected:
    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) = 0;
    virtual u8 get_argc(u8 cmd) const { return 0; }

    void raise_command_irq() { m_state = Irqc; }

public:
    virtual void out8(u8 val) override;
    virtual void out16(u16 val) override;
    virtual u16 in16() override;
    virtual u8 in8() override;
    virtual bool irq_raised() const override;

    virtual u16* reg(u8 id) = 0;

    virtual std::string name() const override { return "Cx16 Conventional Device"; }

    virtual void boot() override;

private:
    struct Command
    {
        u8 m_command;
        std::vector<u16> m_arg_buf;
    };

    State m_state = Ready;
    u8 m_args_needed = 0;
    std::queue<Command> m_pending_commands;
    Command m_current_command;
    u16 m_command_result;
    std::mutex m_queue_access_mutex;
};

class Cx16InterruptController : public Cx16ConventionalDevice
{
public:
    void register_device(u8 irq, std::shared_ptr<Cx16Device> device);

    virtual void boot();

    virtual bool irq_raised() const
    {
        return m_irq_raised || (m_irqc && m_irqc->irq_raised());
    }

    u8 number() const { return m_current_irq + m_offset; }

    virtual std::string name() const override { return "Cx16 Interrupt Controller"; }

    // TODO: Commands
    virtual u8 di_caps() const { return 0x0; }

protected:
    virtual u16* reg(u8 id);
    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args);
    virtual u8 get_argc(u8 cmd) const;

private:
    void eoi();

    std::mutex m_irq_access_mutex;
    std::map<u8, std::shared_ptr<Cx16Device>> m_devices;
    u8 m_current_irq = 0x7; // Cascaded IRQ
    u8 m_offset = 8;
    u16 m_mask = 0xFFFF;
    bool m_irq_raised = false;
};
