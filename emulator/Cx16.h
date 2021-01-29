#pragma once

#include "Config.h"

class Bus;

class Device
{
public:
    virtual std::string name() const { return "Generic Device"; }

    void set_bus(Bus* bus) { m_bus = bus; }

protected:
    Bus* m_bus;
};

class Bus : public Device
{
public:
    void register_device(u8 id, std::shared_ptr<Device> device);
    virtual bool has_device(u8 id) { return find_device(id); }

    virtual std::string name() const override { return "Generic Bus"; }

protected:
    Device* find_device(u8 id);

private:
    std::map<u8, std::shared_ptr<Device>> m_devices;
};

class Cx16Bus : public Bus
{
public:
    virtual void out8(u8 id, u8 val);
    virtual void out16(u8 id, u16 val);
    virtual u16 in16(u8 id);
    virtual u8 in8(u8 id);

    virtual std::string name() const override { return "Cx16 Bus"; }
};

class Cx16IRQCapableDevice;

class Cx16InterruptController : public Device
{
public:
    void register_device(u8 irq, std::shared_ptr<Cx16IRQCapableDevice> device);

    virtual std::string name() const override { return "Cx16 Interrupt Controller"; }

private:
    std::map<u8, std::shared_ptr<Cx16IRQCapableDevice>> m_devices;
};

class Cx16IRQCapableDevice : public Device
{
public:
    virtual bool irq_raised() const = 0;

    void set_interrupt_controller(Cx16InterruptController* irqc) { m_irqc = irqc; }

    virtual std::string name() const override { return "Cx16 IRQ Capable Device"; }

protected:
    Cx16InterruptController* m_irqc;
};

class Cx16Device : public Cx16IRQCapableDevice
{
public:
    virtual void out8(u8 val) = 0;
    virtual void out16(u16 val) = 0;
    virtual u16 in16() = 0;
    virtual u8 in8() = 0;

    virtual u8 di_caps() const = 0;
    virtual u8 pmi_command(u8 cmd) { std::cerr << "Unhandled PMI command: " << (int)cmd << std::endl; return 0; }

    virtual std::string name() const override { return "Cx16 Generic Device"; }
};

class Cx16ConventionalDevice : public Cx16Device
{
    enum State
    {
        Ready,
        RegisterReadRq,
        RegisterRead,
        RegisterWriteRq,
        RegisterWrite,
        CommandRq,
        Irqc
    };
protected:
    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) = 0;
    virtual u8 get_argc(u8 cmd) const { return 0; }

public:
    virtual void out8(u8 val) override;
    virtual void out16(u16 val) override;
    virtual u16 in16() override;
    virtual u8 in8() override;
    virtual bool irq_raised() const override;

    virtual u16* reg(u8 id) = 0;

    virtual std::string name() const override { return "Cx16 Conventional Device"; }

private:
    State m_state = Ready;
    u8 m_args_needed = 0;
    u8 m_command = 0;
    u16 m_command_result = 0;
    std::vector<u16> m_arg_buf;
};
