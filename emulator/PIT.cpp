#include "PIT.h"

u16* PIT::reg(u8 id)
{
    switch(id)
    {
    case 0x0:
        return &m_interval;
    case 0x1:
        return &m_ticks_since_boot;
    }
    return nullptr;
}
