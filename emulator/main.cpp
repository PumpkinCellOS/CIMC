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
class Memory : public Cx16Device
{
public:
    u8 read_memory(u16 addr) const { if(addr >= Size) return 0; return m_mem[addr]; }
    void write_memory(u16 addr, u8 val) { if(addr >= Size) return; m_mem[addr] = val; }

    // Not allowed!
    virtual void out8(u8 val) {}
    virtual void out16(u16 val) {}
    virtual u16 in16() { return 0; }
    virtual u8 in8() { return 0; }
    virtual bool irq_raised() const { return false; }

    virtual u8 di_caps() const override { return 0x0; }
    virtual u8 pmi_command(u8 cmd) override { std::cerr << "PMI Command to memory: " << (int)cmd << std::endl; return 0x00; }

private:
    // Intentionally not initialized!
    u8 m_mem[Size];
};

class CPU : public Cx16Device
{
public:

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

    // Initialize hard-coded devices
    Memory<8192> memory;
    GraphicsCard gfx;

    // Broadcast PMI boot
    memory.pmi_command(0x00);
    gfx.pmi_command(0x00);

    // Do some example code
    gfx.out8(GFX_FILL_RECT);
    gfx.out16(0x0101);
    gfx.out16(0x7f3f);
    gfx.out8(0x01);

    gfx.out8(GFX_FILL_RECT);
    gfx.out16(0x0202);
    gfx.out16(0x7d3d);
    gfx.out8(0x00);

    while(1) ;

    return 0;
}
