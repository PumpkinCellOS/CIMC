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
    info("ROM") << "ROM Dump:";
    std::cerr << std::hex << std::uppercase << std::setfill('0');
    while(!image.eof() && counter < size())
    {
        u8 data;
        image.read((char*)&data, 1);
        if(counter % 16 == 0)
            std::cerr << std::endl << std::setw(4) << counter << ": ";
        std::cerr << std::setw(2) << (int)data << " ";
        write_memory(counter, data);
        counter++;
    }
    std::cerr << std::endl << std::endl << std::dec << std::nouppercase << std::setfill(' ');
    m_unlocked = false;
}

void CPU::ROMMemory::write_memory(u16 addr, u8 val)
{
    if(!m_unlocked)
    {
        error("ROM") << "Write to read-only memory, addr=0x" << std::hex << std::setfill('0') << std::setw(8) << addr;
        exit(-1);
    }
    Memory::write_memory(addr, val);
}
