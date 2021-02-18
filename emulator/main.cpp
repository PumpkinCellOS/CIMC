#include "CPU.h"
#include "Cx16.h"
#include "Display.h"
#include "Gfx.h"
#include "HDD.h"
#include "PIT.h"
#include "PMI.h"

#include <unistd.h>

class LegacyIOBus : public Cx16Bus { public: virtual std::string name() const { return "Legacy I/O Bus"; } };
class FastIOBus : public Cx16Bus { public: virtual std::string name() const { return "Fast I/O Bus"; } };

int main()
{
    std::ofstream err("err.log", std::ios::app);
    if(!err.good())
    {
        error("main") << "Failed to open error log!";
    }
    std::cerr.rdbuf(err.rdbuf());

    std::cerr << std::endl << "\e[38;5;68;1mPumpkinCellOS cx16 Emulator v1.0\e[0m" << std::endl << std::endl;

    display::init(128, 64);

    // Create CPU and memory
    auto memory = std::make_shared<Memory>(8192);
    auto cpu = std::make_shared<CPU>();

    // Create devices
    auto cpu_io_bus = std::make_shared<CPUIOBus>();

    // Create busses
    auto legacy_io_bus = std::make_shared<LegacyIOBus>();
    auto fast_io_bus = std::make_shared<FastIOBus>();

    cpu_io_bus->set_master(cpu);
    cpu_io_bus->register_io_switch(legacy_io_bus);
    cpu_io_bus->register_io_switch(fast_io_bus);

    // Create DMA controllers
    auto legacy_dma_controller = std::make_shared<Cx16DMAController>();
    legacy_dma_controller->set_master(memory.get());
    cpu_io_bus->set_dma_controller(legacy_dma_controller);

    // Create end devices
    auto pic = std::make_shared<Cx16InterruptController>();
    cpu_io_bus->set_interrupt_controller(pic);

    auto pit = std::make_shared<PIT>(); // 0x00
    auto pmi = std::make_shared<PMIController>(); // 0x05
    auto gfx = std::make_shared<GraphicsCard>();  // 0x0F

    // Load disk image
    info("main") << "Loading disk image";
    std::ifstream disk_image("disk.img");

    auto mass_storage = std::make_shared<MassStorage>(disk_image);

    // Register devices
    pic->register_device(0x00, pit);
    pic->register_device(0x02, pmi);
    pic->register_device(0x03, mass_storage);

    fast_io_bus->register_device(0x0F, gfx);
    legacy_io_bus->register_device(0x00, pit);
    legacy_io_bus->register_device(0x02, mass_storage);
    legacy_io_bus->register_device(0x04, pic);
    legacy_io_bus->register_device(0x05, pmi);

    legacy_dma_controller->register_device(0x02, mass_storage);
    legacy_dma_controller->register_device(0x0F, gfx);

    pmi->register_device(gfx);
    pmi->register_device(mass_storage);
    pmi->register_device(memory);
    pmi->register_device(cpu);

    // BOOT here.
    pmi->power_button();

    info("main") << "Initialization done.";

    // Reboot test.
    while(true) { sleep(10); pmi->reset_button(); }

    return 0;
}
