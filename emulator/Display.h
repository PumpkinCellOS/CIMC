#pragma once

#include "Config.h"

namespace display
{

void init(u8 sx, u8 sy);
void set_pixel(u8 x, u8 y, u8 color);
u8 pixel(u8 x, u8 y);

}
