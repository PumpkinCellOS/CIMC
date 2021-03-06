#include "Gfx.h"

#include "Display.h"

u16 GraphicsCard::do_cmd(u8 cmd, const std::vector<u16>& args)
{
    switch(cmd)
    {
        case GFX_CMD_CLEAR_FB: return clear_framebuffer(args[0]); break;
        case GFX_CMD_SET_PIXEL: return set_pixel(args[0], args[1]); break;
        case GFX_CMD_FILL_RECT: return fill_rect(args[0], args[1], args[2]); break;
        case GFX_CMD_FLUSH_RECT: return flush_rect(args[0], args[1]); break;
        case GFX_CMD_FILL_FB: return fill_framebuffer_from_memory(args[0], args[1], args[2]); break;
    }
    return 0;
}

u8 GraphicsCard::get_argc(u8 cmd) const
{
    switch(cmd)
    {
        case GFX_CMD_CLEAR_FB: return 1;
        case GFX_CMD_SET_PIXEL: return 2;
        case GFX_CMD_FILL_RECT: return 3;
        case GFX_CMD_FLUSH_RECT: return 2;
        case GFX_CMD_FILL_FB: return 3;
        default: return Cx16ConventionalDevice::get_argc(cmd);
    }
}

u8 GraphicsCard::di_caps() const
{
    return 0x7;
}

u16* GraphicsCard::reg(u8 id)
{
    switch(id)
    {
        case GFX_REG_VIDEOMODE: return &video_mode;
        case GFX_REG_SCREENSIZE: return &screen_size;
        case GFX_REG_FBADDR: return &fb_addr;
    }
    return nullptr;
}

u16 GraphicsCard::clear_framebuffer(u8 color)
{
    return fill_rect(0x0000, 0x8040, color);
}

u16 GraphicsCard::set_pixel(u16 coords, u8 color)
{
    display::set_pixel((coords & 0xFF00) >> 8, coords & 0xFF, color);
    return 0;
}

u16 GraphicsCard::fill_rect(u16 coords, u16 size, u8 color)
{
    u8 xs = (coords & 0xFF00) >> 8;
    u8 ys = coords & 0xFF;
    u8 xe = (size & 0xFF00) >> 8;
    u8 ye = size & 0xFF;
    trace("Gfx") << "fill_rect: " << (int)xs << "," << (int)ys << ":" << (int)xe << "," << (int)ye << " = " << (int)color;

    for(u8 x = xs; x < xs + xe; x++)
    for(u8 y = ys; y < ys + ye; y++)
        display::set_pixel(x, y, color);

    return 0;
}

u16 GraphicsCard::flush_rect(u16 coords, u16 size)
{
    return 0;
}

u16 GraphicsCard::fill_framebuffer_from_memory(u16 coords, u16 size, u8 op)
{
    u8 xs = (coords & 0xFF00) >> 8;
    u8 ys = coords & 0xFF;
    u8 xe = (size & 0xFF00) >> 8;
    u8 ye = size & 0xFF;
    trace("Gfx") << "fill_framebuffer_from_memory: " << (int)xs << "," << (int)ys << ":" << (int)xe << "," << (int)ye;

    for(u8 x = xs; x < xs + xe; x++)
    for(u8 y = ys; y < ys + ye; y++)
    {
        u8 current_pixel = display::pixel(x, y);
        u8 new_pixel = m_dmac->read_memory(fb_addr + (x - xs) * ye + (y - ys));
        switch(op)
        {
        case GFX_FILL_OP_ASSIGN:
            current_pixel = new_pixel;
            break;
        case GFX_FILL_OP_AND:
            current_pixel &= new_pixel;
            break;
        case GFX_FILL_OP_OR:
            current_pixel |= new_pixel;
            break;
        case GFX_FILL_OP_XOR:
            current_pixel ^= new_pixel;
            break;
        case GFX_FILL_OP_XNOR:
            current_pixel ^= ~new_pixel;
            break;
        }
        display::set_pixel(x, y, current_pixel);
    }

    return 0;
}
