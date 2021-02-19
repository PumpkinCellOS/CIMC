#include "CPU.h"

#include "Config.h"
#include "Gfx.h"

#include <unistd.h>

void CPU::boot()
{
    info("CPU") << "Booting up CPU #0";

    while(!running())
    {
        while(!m_ready) ;
        m_ready = false;

        // wait for all devices to boot...
        sleep(1);

        info("CPU") << "Devices ready, starting ROM execution.";

        while(true)
        {
            m_control_unit.cycle();
            m_control_unit.dump_registers();
        }
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

void CPU::ROMMemory::init_with_image(std::istream& image)
{
    size_t counter = 0;
    while(!image.eof() && counter < size())
    {
        u8 data;
        image.read((char*)&data, 1);
        write_memory(counter, data);
        counter++;
    }
}

// INSTRUCTIONS


