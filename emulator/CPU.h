#pragma once

#include "Cx16.h"

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

private:
    std::shared_ptr<Cx16Master> m_master;
    std::shared_ptr<Cx16InterruptController> m_interrupt_controller;
    std::shared_ptr<Cx16DMAController> m_dma_controller;
};

class CPU : public Cx16Master, public PMICapableDevice
{
public:
    virtual std::string name() const { return "PumpkinCellOS cx16 Emulated CPU"; }

    virtual void boot() override;

    // Instructions
    void insn_out8(u8 id, u8 val);
    void insn_out16(u8 id, u16 val);
    u8 insn_in8(u8 id);
    u16 insn_in16(u8 id);
    // ...
    void insn_hlt();

    bool irq_raised() { return slave()->interrupt_controller()->irq_raised(); }

    // TODO
    void write_virtual_memory(u16 addr, u8 value) { write_physical_memory(addr, value); }
    u8 read_virtual_memory(u16 addr) { return read_virtual_memory(addr); }

    void pmi_boot();
    void pmi_reboot();

private:
    void write_physical_memory(u16 addr, u8 value) { slave()->dma_controller()->write_memory(addr, value); }
    u8 read_physical_memory(u16 addr) { return slave()->dma_controller()->read_memory(addr); }

    CPUIOBus* slave() { return (CPUIOBus*)m_slave; }

    bool m_ready = false;
};
