#include "Display.h"

#include "Config.h"

namespace display
{

static u8* display_buffer = nullptr;
static u8 size_x = 0, size_y = 0;

void init(u8 sx, u8 sy)
{
    info("Display") << "Initializing: " << (int)sx << "x" << (int)sy;

    // Hide Cursor
    std::cout << "\033[?25l";

    system("stty -echo");

    size_x = sx;
    size_y = sy;
    display_buffer = new u8[size_x * size_y];

    for(u8 x = 0; x < sx; x++)
    for(u8 y = 0; y < sy; y++)
        set_pixel(x, y, 0);
}

void set_pixel(u8 x, u8 y, u8 color)
{
    if(!display_buffer)
    {
        error("Display") << "set_pixel: Display not initialized";
        return;
    }

    // Set Cursor
    std::cout << "\e[" << (int)y << ";" << (int)(x*2-1) << "f";

    // Display
    std::cout << (color ? "\e[107m" : "\e[40m") << "  " << std::flush;

    // Set internal buffer
    display_buffer[x*size_y+y] = color;
}

u8 pixel(u8 x, u8 y)
{
    if(!display_buffer)
    {
        error("Display") << "pixel: Display not initialized";
        return 0;
    }
    return display_buffer[x*size_y+y];
}

}
