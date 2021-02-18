#include "CPU.h"

#include "Config.h"
#include "Gfx.h"

void CPU::boot()
{
    info("CPU") << "Booting up CPU #0";

    while(!running())
    {
        while(!m_ready) ;
        m_ready = false;

        info("CPU") << "Devices ready, starting ROM execution.";

        while(true)
            m_control_unit.cycle();
    }
}

void CPU::pmi_boot()
{
    m_ready = true;
}

void CPU::pmi_reboot()
{
    m_ready = false;
}

// INSTRUCTIONS


