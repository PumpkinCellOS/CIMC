#include "CPU.h"
#include "Cx16.h"

#define GFX_CLEAR_FB 0x02
#define GFX_SET_PIXEL 0x03
#define GFX_FILL_RECT 0x04
#define GFX_FLUSH_RECT 0x05
#define GFX_FILL_FB 0x06

namespace display
{

void init(u8 sx, u8 sy);
void set_pixel(u8 x, u8 y, u8 color);

void init(u8 sx, u8 sy)
{
    // Hide Cursor
    std::cout << "\033[?25l";

    system("stty -echo");

    for(u8 x = 0; x < sx; x++)
    for(u8 y = 0; y < sy; y++)
        set_pixel(x, y, 0);
}

void set_pixel(u8 x, u8 y, u8 color)
{
    // Set Cursor
    std::cout << "\e[" << (int)y << ";" << (int)(x*2-1) << "f";

    // Display
    std::cout << (color ? "\e[107m" : "\e[40m") << "  " << std::flush;
}

}

//size: 128x64
class GraphicsCard : public Cx16ConventionalDevice
{
public:
    virtual std::string name() const override { return "Cx16 Graphics Adapter"; }

protected:
    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) override
    {
        switch(cmd)
        {
            case 0x02: return clear_framebuffer(args[0]); break;
            case 0x03: return set_pixel(args[0], args[1]); break;
            case 0x04: return fill_rect(args[0], args[1], args[2]); break;
            case 0x05: return flush_rect(args[0], args[1]); break;
            case 0x06: return fill_framebuffer_from_memory(args[0], args[1], args[2]); break;
        }
        return 0;
    }

    virtual u8 get_argc(u8 cmd) const override
    {
        switch(cmd)
        {
            case 0x02: return 1;
            case 0x03: return 2;
            case 0x04: return 3;
            case 0x05: return 2;
            case 0x06: return 3;
            default: return Cx16ConventionalDevice::get_argc(cmd);
        }
    }

    virtual u8 di_caps() const override
    {
        return 0x7;
    }

    virtual u16* reg(u8 id) override
    {
        switch(id)
        {
            case 0x00: return &video_mode;
            case 0x01: return &screen_size;
            case 0x02: return &fb_addr;
        }
        return nullptr;
    }

private:
    u16 clear_framebuffer(u8 color)
    {
        return fill_rect(0x0000, 0x8040, color);
    }
    u16 set_pixel(u16 coords, u8 color)
    {
        display::set_pixel((coords & 0xFF00) >> 8, coords & 0xFF, color);
        return 0;
    }
    u16 fill_rect(u16 coords, u16 size, u8 color)
    {
        u8 xs = (coords & 0xFF00) >> 8;
        u8 ys = coords & 0xFF;
        u8 xe = (size & 0xFF00) >> 8;
        u8 ye = size & 0xFF;
        std::cerr << "fill_rect: " << (int)xs << "," << (int)ys << ":" << (int)xe << "," << (int)ye << std::endl;

        for(u8 x = xs; x < xs + xe; x++)
        for(u8 y = ys; y < ys + ye; y++)
            display::set_pixel(x, y, color);

        return 0;
    }
    u16 flush_rect(u16 coords, u16 size)
    {
        return 0;
    }
    u16 fill_framebuffer_from_memory(u16 coords, u16 size, u8 op)
    {
        return 0;
    }

    u16 video_mode = 0x0;
    u16 screen_size = 0x8040;
    u16 fb_addr = 0x0;
};

template<u16 Size>
class Memory : public Device
{
public:
    Memory() {}

    u8 read_memory(u16 addr) const { if(addr >= Size) return 0; return m_mem[addr]; }
    void write_memory(u16 addr, u8 val) { if(addr >= Size) return; m_mem[addr] = val; }

    virtual std::string name() const override { return "Main Memory"; }

private:
    // Intentionally not initialized!
    u8 m_mem[Size];
};

typedef Memory<8192> CIMCMemory;

class Cx16CPUBus : public Cx16Bus
{
public:
    void register_io_switch(std::shared_ptr<Cx16Bus> bus)
    {
        std::cerr << "Cx16CPUBus: Registering I/O Switch bus: " << bus->name() << std::endl;
        m_switches.push_back(bus);
    }

    virtual void out8(u8 id, u8 val) override
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                sw->out8(id, val);
        }
    }

    virtual void out16(u8 id, u16 val) override
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                sw->out16(id, val);
        }
    }

    virtual u16 in16(u8 id) override
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                return sw->in16(id);
        }
        return 0;
    }

    virtual u8 in8(u8 id) override
    {
        for(auto sw: m_switches)
        {
            if(sw->has_device(id))
                return sw->in8(id);
        }
        return 0;
    }

    virtual std::string name() const override { return "Cx16 CPU Bus"; }

private:
    std::vector<std::shared_ptr<Cx16Bus>> m_switches;
};

int main()
{
    std::ofstream err("err.log", std::ios::app);
    if(!err.good())
    {
        std::cerr << "Failed to open error log!" << std::endl;
        return 1;
    }
    std::cerr.rdbuf(err.rdbuf());
    display::init(128, 64);

    // Create CPU and memory
    auto memory = std::make_shared<CIMCMemory>();

    // Create devices
    auto cpu_io_bus = std::make_shared<Cx16CPUBus>();

    auto legacy_io_bus = std::make_shared<Cx16Bus>();
    auto slow_io_bus = std::make_shared<Cx16Bus>();
    auto fast_io_bus = std::make_shared<Cx16Bus>();

    cpu_io_bus->register_io_switch(legacy_io_bus);
    cpu_io_bus->register_io_switch(slow_io_bus);
    cpu_io_bus->register_io_switch(fast_io_bus);

    auto pic = std::make_shared<Cx16InterruptController>();
    auto gfx = std::make_shared<GraphicsCard>();
    pic->register_device(0x00, gfx);

    legacy_io_bus->register_device(0x0F, gfx); // TODO: Move to Fast I/O
    legacy_io_bus->register_device(0x04, pic);

    // Do some example code
    cpu_io_bus->out8(0x0F, GFX_FILL_RECT);
    cpu_io_bus->out16(0x0F, 0x0101);
    cpu_io_bus->out16(0x0F, 0x7f3f);
    cpu_io_bus->out8(0x0F, 0x01);

    cpu_io_bus->out8(0x0F, GFX_FILL_RECT);
    cpu_io_bus->out16(0x0F, 0x0202);
    cpu_io_bus->out16(0x0F, 0x7d3d);
    cpu_io_bus->out8(0x0F, 0x00);

    while(1) ;

    return 0;
}
