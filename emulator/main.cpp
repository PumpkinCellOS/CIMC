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

    // Load ROM image
    info("main") << "Loading ROM image";
    std::ifstream rom_image("rom.img");

    // Create CPU and memory
    auto memory = std::make_shared<Memory>(8192);

    auto cpu = std::make_shared<CPU>(rom_image);

    if(!rom_image.good())
    {
        error("main") << "Failed to load ROM image. Try adding rom.img file in current directory.";
        return 1;
    }

    rom_image.close();

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
    auto fast_io_pic = std::make_shared<Cx16InterruptController>();
    cpu_io_bus->set_interrupt_controller(fast_io_pic);

    auto legacy_pic = std::make_shared<Cx16InterruptController>();
    legacy_pic->set_interrupt_controller(legacy_pic.get());
    fast_io_pic->register_device(0x05, legacy_pic); // Cascading IRQ

    auto pit = std::make_shared<PIT>(); // I/O 0x00, IRQ 0
    // TODO:
    // auto keyboard ; // I/O 0x01, IRQ 1
    // auto soundcard ; // I/O 0x03, IRQ 3
    auto pmi = std::make_shared<PMIController>(); // I/O 0x05, IRQ 2
    // TODO:
    // auto randomizer ; // I/O 0x06
    // auto rtc ; // I/O 0x07
    // auto device_info ; // I/O 0x09
    // auto printer ; // I/O 0x0e, IRQ 9
    auto gfx = std::make_shared<GraphicsCard>(); // I/O 0x0F, IRQ 10

    // Load disk image
    info("main") << "Loading disk image";
    std::ifstream disk_image("disk.img");
    if(!disk_image.good())
    {
        // TODO: In this case, simply disable the disk.
        error("main") << "Failed to load disk image. Try adding disk.img file in current directory.";
        return 1;
    }

    auto mass_storage = std::make_shared<MassStorage>(disk_image);

    // Register devices
    legacy_pic->register_device(0x00, pit);
    legacy_pic->register_device(0x02, pmi);
    legacy_pic->register_device(0x03, mass_storage);

    fast_io_bus->register_device(0x0C, fast_io_pic);
    //fast_io_bus->register_device(0x0E, printer); // Printer
    fast_io_bus->register_device(0x0F, gfx);

    legacy_io_bus->register_device(0x00, pit);
    legacy_io_bus->register_device(0x02, mass_storage);
    // TODO:
    // Soundcard (0x03)
    legacy_io_bus->register_device(0x04, legacy_pic);
    legacy_io_bus->register_device(0x05, pmi);
    // TODO:
    // Randomizer (0x06)
    // RTC (0x07)

    legacy_dma_controller->register_device(0x02, mass_storage);
    legacy_dma_controller->register_device(0x0F, gfx);

    pmi->register_device(gfx);
    pmi->register_device(mass_storage);
    pmi->register_device(memory);
    pmi->register_device(cpu);

    // "Connect the power" to devices (create threads)
    info("main") << "Creating device worker threads.";
    for(auto device: Device::all_devices)
        device->power_on();

    // BOOT here.
    pmi->power_button();

    info("main") << "Initialization done.";

    // Reboot test.
    while(true) { sleep(10); pmi->reset_button(); }

    return 0;
}
