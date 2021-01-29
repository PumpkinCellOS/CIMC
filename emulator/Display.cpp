#include "Display.h"

namespace display
{

static u8* display_buffer = nullptr;
static u8 size_x = 0, size_y = 0;

void init(u8 sx, u8 sy)
{
    std::cerr << "Initializing Display: " << (int)sx << "x" << (int)sy << std::endl;

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
        std::cerr << "set_pixel: Display not initialized" << std::endl;
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
        std::cerr << "pixel: Display not initialized" << std::endl;
        return 0;
    }
    return display_buffer[x*size_y+y];
}

}
