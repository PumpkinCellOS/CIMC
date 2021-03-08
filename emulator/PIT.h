#include "Cx16.h"

// TODO: Actually be a timer :^)
class PIT : public Cx16ConventionalDevice
{
public:
    virtual u16 do_cmd(u8, const std::vector<u16>&) override { return 0x0; }
    virtual u16* reg(u8 id) override;

    virtual u8 di_caps() const override { return 0x0; }

    virtual std::string name() const override { return "Cx16 Programmable Interval Timer"; }

private:
    u16 m_interval = 1;
    u16 m_ticks_since_boot = 0;
};
