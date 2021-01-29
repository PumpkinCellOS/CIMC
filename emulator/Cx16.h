#pragma once

#include "Config.h"

class Device
{
public:
    virtual void out8(u8 val) = 0;
    virtual void out16(u16 val) = 0;
    virtual u16 in16() = 0;
    virtual u8 in8() = 0;
    virtual bool irq_raised() const = 0;
};

class Cx16Device : public Device
{
public:
    virtual u8 di_caps() const = 0;
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

private:
    State m_state = Ready;
    u8 m_args_needed = 0;
    u8 m_command = 0;
    u16 m_command_result = 0;
    std::vector<u16> m_arg_buf;
};
