#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;

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

class Device
{
public:
    virtual void out8(u8 val) = 0;
    virtual void out16(u16 val) = 0;
    virtual u16 in16() = 0;
    virtual u8 in8() = 0;
    virtual bool irq_raised() const = 0;
};

class Cx16IODevice : public Device
{
    enum State
    {
        Ready,
        RegisterReadRq,
        RegisterRead,
        RegisterWriteRq,
        RegisterWrite,
        CommandRq,
        Irqc
    };
protected:
    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) = 0;
    virtual u8 get_argc(u8 cmd) const { return 0; }

public:
    virtual void out8(u8 val) override
    {
        switch(m_state)
        {
        case Ready:
            {
                switch(val)
                {
                    case 0x00: m_state = RegisterReadRq; break;
                    case 0x01: m_state = RegisterWriteRq; break;
                    default:
                        m_args_needed = get_argc(val);
                        m_command = val;
                        m_state = CommandRq;
                        std::cerr << (int)m_args_needed << " " << (int)m_command << " " << (int)m_state << std::endl;
                        break;
                }
            } break;
        case RegisterReadRq:
            m_command = val;
            m_state = RegisterRead;
            break;
        case RegisterWriteRq:
            m_command = val;
            m_state = RegisterWrite;
            break;
        case CommandRq:
            out16(val);
            break;
        default:
            break;
        }
    }
    virtual void out16(u16 val) override
    {
        switch(m_state)
        {
        case CommandRq:
            m_arg_buf.push_back(val);
            if(!(--m_args_needed))
            {
                std::cerr << "Cx16Device command: " << (int)m_command << std::endl;
                m_command_result = do_cmd(m_command, m_arg_buf);
                m_arg_buf.clear();
                m_state = Ready;
            }
            break;
        case RegisterWrite:
            {
                u16* _reg = reg(m_command);
                if(_reg)
                    *_reg = val;
                m_state = Ready;
            } break;
        case RegisterRead:
            {
                u16* _reg = reg(m_command);
                if(_reg)
                    m_command_result = *_reg;
                m_state = Ready;
            } break;
        default:
            break;
        }
    }
    virtual u16 in16() override
    {
        return m_command_result;
    }
    virtual u8 in8() override
    {
        // TODO: async commands!
        if(m_state == Irqc)
            return m_command;
        return 0;
    }
    virtual bool irq_raised() const override
    {
        // TODO: async commands!
        return false;
    }

    virtual u16* reg(u8 id) = 0;

    virtual u8 di_caps() const = 0;

private:
    State m_state = Ready;
    u8 m_args_needed = 0;
    u8 m_command = 0;
    u16 m_command_result = 0;
    std::vector<u16> m_arg_buf;
};

//size: 128x64
class GraphicsCard : public Cx16IODevice
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
            default: return Cx16IODevice::get_argc(cmd);
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

    GraphicsCard gfx;

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
