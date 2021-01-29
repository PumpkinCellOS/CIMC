#pragma once

#include "Config.h"
#include "Cx16.h"

#define GFX_CMD_CLEAR_FB 0x02
#define GFX_CMD_SET_PIXEL 0x03
#define GFX_CMD_FILL_RECT 0x04
#define GFX_CMD_FLUSH_RECT 0x05
#define GFX_CMD_FILL_FB 0x06

#define GFX_FILL_OP_ASSIGN 0x00
#define GFX_FILL_OP_AND    0x01
#define GFX_FILL_OP_OR     0x02
#define GFX_FILL_OP_XOR    0x03
#define GFX_FILL_OP_XNOR   0x04

#define GFX_REG_VIDEOMODE 0x00
#define GFX_REG_SCREENSIZE 0x01
#define GFX_REG_FBADDR 0x02

//size: 128x64
class GraphicsCard : public Cx16ConventionalDevice
{
public:
    virtual std::string name() const override { return "Cx16 Graphics Adapter"; }

protected:
    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) override;
    virtual u8 get_argc(u8 cmd) const override;
    virtual u8 di_caps() const override;
    virtual u16* reg(u8 id) override;

private:
    u16 clear_framebuffer(u8 color);
    u16 set_pixel(u16 coords, u8 color);
    u16 fill_rect(u16 coords, u16 size, u8 color);
    u16 flush_rect(u16 coords, u16 size);
    u16 fill_framebuffer_from_memory(u16 coords, u16 size, u8 op);

    u16 video_mode = 0x0;
    u16 screen_size = 0x8040;
    u16 fb_addr = 0x0;
};
