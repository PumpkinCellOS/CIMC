#pragma once

#include "Cx16.h"

#define PMI_CMD_SHUTDOWN 0x02
#define PMI_CMD_REBOOT   0x03

class PMIController : public Cx16ConventionalDevice
{
public:
    virtual std::string name() const override { return "Cx16 PMI Controller"; }

    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) override;
    virtual u8 get_argc(u8 cmd) const override;
    virtual u16* reg(u8 id) override;

    virtual u8 di_caps() const override { return 0x1; }

    virtual void register_device(std::shared_ptr<PMICapableDevice> device);

    void power_button();
    void reset_button();

    bool hdd_led_on() { return false; }
    bool power_led_on() const { return m_power_led_on; }

private:
    void reboot();
    void shutdown();

    std::vector<std::shared_ptr<PMICapableDevice>> m_devices;
    bool m_power_led_on = false;
};
