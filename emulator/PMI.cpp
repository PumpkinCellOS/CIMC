#include "PMI.h"

u16 PMIController::do_cmd(u8 cmd, const std::vector<u16>& args)
{
    switch(cmd)
    {
        case PMI_CMD_SHUTDOWN:
            shutdown();
            break;
        case PMI_CMD_REBOOT:
            reboot();
            break;
    }
    return 0x0;
}

u8 PMIController::get_argc(u8 cmd) const
{
    switch(cmd)
    {
        case PMI_CMD_SHUTDOWN:
        case PMI_CMD_REBOOT:
            return 0;
            break;
        default:
            return Cx16ConventionalDevice::get_argc(cmd);
    }
}

u16* PMIController::reg(u8 id)
{
    return nullptr;
}

void PMIController::reboot()
{
    info("PMIController") << "Reboot on " << m_devices.size() << " devices";
    for(auto& dev : m_devices)
        dev->pmi_reboot();

    // ...

    for(auto& dev : m_devices)
        dev->pmi_boot();
}

void PMIController::shutdown()
{
    info("PMIController") << "Shutdown on " << m_devices.size() << " devices";
    for(auto& dev : m_devices)
        dev->pmi_shutdown();
    m_power_led_on = false;
}

void PMIController::reset_button()
{
    debug("PMIController") << "reset_button() on " << m_devices.size() << " devices";
    reboot();
}

void PMIController::power_button()
{
    info("PMIController") << "power_button() on " << m_devices.size() << " devices";
    m_power_led_on = true;
    for(auto& dev : m_devices)
        dev->pmi_boot();
}

void PMIController::register_device(std::shared_ptr<PMICapableDevice> device)
{
    device->set_pmi_controller(this);
    m_devices.push_back(device);
}
