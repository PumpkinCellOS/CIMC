#include "CPU.h"
#include "Cx16.h"
#include "Display.h"
#include "Gfx.h"
#include "HDD.h"

#include <unistd.h>

class LegacyIOBus : public Cx16Bus { public: virtual std::string name() const { return "Legacy I/O Bus"; } };
class SlowIOBus : public Cx16Bus { public: virtual std::string name() const { return "Slow I/O Bus"; } };
class FastIOBus : public Cx16Bus { public: virtual std::string name() const { return "Fast I/O Bus"; } };
class CPUIOBus : public Cx16Bus { public: virtual std::string name() const { return "CPU I/O Bus"; } };

int main()
{
    std::ofstream err("err.log", std::ios::app);
    if(!err.good())
    {
        log("main") << "Failed to open error log!";
    }
    std::cerr.rdbuf(err.rdbuf());

    std::cerr << std::endl << "\e[38;5;68;1mPumpkinCellOS cx16 Emulator v1.0\e[0m" << std::endl << std::endl;

    display::init(128, 64);

    // Create CPU and memory
    auto memory = std::make_shared<Memory>(8192);

    // Create devices
    auto cpu_io_bus = std::make_shared<CPUIOBus>();

    // Create busses
    auto legacy_io_bus = std::make_shared<LegacyIOBus>();
    auto slow_io_bus = std::make_shared<SlowIOBus>();
    auto fast_io_bus = std::make_shared<FastIOBus>();

    cpu_io_bus->register_io_switch(legacy_io_bus);
    cpu_io_bus->register_io_switch(slow_io_bus);
    cpu_io_bus->register_io_switch(fast_io_bus);

    // Create DMA controllers
    auto legacy_dma_controller = std::make_shared<Cx16DMAController>();
    legacy_dma_controller->set_master(memory.get());

    // Create end devices
    auto pic = std::make_shared<Cx16InterruptController>();
    auto gfx = std::make_shared<GraphicsCard>();

    // Register devices
    pic->register_device(0x00, gfx);

    fast_io_bus->register_device(0x0F, gfx);
    legacy_io_bus->register_device(0x04, pic);

    legacy_dma_controller->register_device(0x0F, gfx);

    // Do some example code
    cpu_io_bus->out8(0x0F, GFX_CMD_FILL_RECT);
    cpu_io_bus->out16(0x0F, 0x0101);
    cpu_io_bus->out16(0x0F, 0x7f3f);
    cpu_io_bus->out8(0x0F, 0x01);

    cpu_io_bus->out8(0x0F, GFX_CMD_FILL_RECT);
    cpu_io_bus->out16(0x0F, 0x0202);
    cpu_io_bus->out16(0x0F, 0x7d3d);
    cpu_io_bus->out8(0x0F, 0x00);

    // Test registers
    cpu_io_bus->out8(0x0F, CX16_REG_WRITE);
    cpu_io_bus->out8(0x0F, 0x02);
    cpu_io_bus->out16(0x0F, 0x1000);

    // Test DMA
    cpu_io_bus->out8(0x0F, GFX_CMD_FILL_FB);
    cpu_io_bus->out16(0x0F, 0x0303);
    cpu_io_bus->out16(0x0F, 0x0a0a);
    cpu_io_bus->out8(0x0F, GFX_FILL_OP_XNOR);

    cpu_io_bus->out8(0x0F, GFX_CMD_FILL_FB);
    cpu_io_bus->out16(0x0F, 0x0a0a);
    cpu_io_bus->out16(0x0F, 0x0a0a);
    cpu_io_bus->out8(0x0F, GFX_FILL_OP_XNOR);

    cpu_io_bus->out8(0x0F, GFX_CMD_FILL_FB);
    cpu_io_bus->out16(0x0F, 0x1111);
    cpu_io_bus->out16(0x0F, 0x0a0a);
    cpu_io_bus->out8(0x0F, GFX_FILL_OP_XNOR);

    log("main") << "Main thread finished";

    while(1) ;

    return 0;
}
