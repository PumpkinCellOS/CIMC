#include "CPU.h"

#include "Config.h"
#include "Gfx.h"

void CPU::boot()
{
    info("CPU") << "Booting up CPU #0";

    while(!running())
    {
        while(!m_ready) ;
        m_ready = false;

        info("CPU") << "Devices ready, starting ROM execution.";

        // Do some example code
        insn_out8(0x0F, GFX_CMD_FILL_RECT);
        insn_out16(0x0F, 0x0101);
        insn_out16(0x0F, 0x7f3f);
        insn_out8(0x0F, 0x01);

        insn_out8(0x0F, GFX_CMD_FILL_RECT);
        insn_out16(0x0F, 0x0202);
        insn_out16(0x0F, 0x7d3d);
        insn_out8(0x0F, 0x00);

        // Test registers
        insn_out8(0x0F, CX16_REG_WRITE);
        insn_out8(0x0F, 0x02);
        insn_out16(0x0F, 0x1000);

        // Test DMA
        insn_out8(0x0F, GFX_CMD_FILL_FB);
        insn_out16(0x0F, 0x0303);
        insn_out16(0x0F, 0x0a0a);
        insn_out8(0x0F, GFX_FILL_OP_XNOR);

        insn_out8(0x0F, GFX_CMD_FILL_FB);
        insn_out16(0x0F, 0x0a0a);
        insn_out16(0x0F, 0x0a0a);
        insn_out8(0x0F, GFX_FILL_OP_XNOR);

        insn_out8(0x0F, GFX_CMD_FILL_FB);
        insn_out16(0x0F, 0x1111);
        insn_out16(0x0F, 0x0a0a);
        insn_out8(0x0F, GFX_FILL_OP_XNOR);
    }
}

void CPU::pmi_boot()
{
    m_ready = true;
}

void CPU::pmi_reboot()
{
    m_ready = false;
}

// INSTRUCTIONS

void CPU::insn_out8(u8 id, u8 val)
{
    slave()->out8(id, val);
}

void CPU::insn_out16(u8 id, u16 val)
{
    slave()->out16(id, val);
}

u8 CPU::insn_in8(u8 id)
{
    return slave()->in8(id);
}

u16 CPU::insn_in16(u8 id)
{
    return slave()->in16(id);
}

void CPU::insn_hlt()
{
    m_device_destroyed.store(true);
}
