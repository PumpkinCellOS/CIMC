#pragma once

#include "Cx16.h"
#include "ControlUnit.h"

class CPUIOBus : public Cx16Bus
{
public:
    virtual std::string name() const
    {
        return "CPU I/O Bus";
    }

    void set_master(std::shared_ptr<Cx16Master> bus)
    {
        info(name()) << "Setting I/O Master bus: " << bus->name();
        bus->set_slave(this);
        m_master = bus;
    }

    void set_interrupt_controller(std::shared_ptr<Cx16InterruptController> ctr)
    {
        info(name()) << "Setting IRQ controller: " << ctr->name();
        m_interrupt_controller = ctr;
    }

    void set_dma_controller(std::shared_ptr<Cx16DMAController> dma)
    {
        info(name()) << "Setting DMA controller: " << dma->name();
        m_dma_controller = dma;
    }

    Cx16InterruptController* interrupt_controller() { return m_interrupt_controller.get(); }
    Cx16DMAController* dma_controller() { return m_dma_controller.get(); }
    const Cx16DMAController* dma_controller() const { return m_dma_controller.get(); }

private:
    std::shared_ptr<Cx16Master> m_master;
    std::shared_ptr<Cx16InterruptController> m_interrupt_controller;
    std::shared_ptr<Cx16DMAController> m_dma_controller;
};

class CPU : public Cx16Master, public PMICapableDevice
{
public:
    class ROMMemory : Memory
    {
    public:
        ROMMemory()
        : Memory(384) {}

        virtual std::string name() const override { return "ROM"; }
    };

    virtual std::string name() const { return "PumpkinCellOS cx16 Emulated CPU"; }

    virtual void boot() override;

    // Instructions

    bool irq_raised() { return slave()->interrupt_controller()->irq_raised(); }

    // TODO
    u16 map_virtual_to_physical(u16 virt) const { return virt; }

    void pmi_boot();
    void pmi_reboot();

    CPUIOBus* slave() { return (CPUIOBus*)m_slave; }
    ROMMemory& rom() { return m_rom; }

private:
    bool m_ready = false;

    ControlUnit m_control_unit = { *this };

    ROMMemory m_rom;
};
