#include "HDD.h"

virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) override;
virtual u8 get_argc(u8 cmd) const override;
virtual u16* reg(u8 id) override;
