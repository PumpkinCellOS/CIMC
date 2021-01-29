#pragma once

#include "Config.h"
#include "Cx16.h"

#define HDD_REG_FILLED 0x00
#define HDD_REG_IDC    0x01
#define HDD_REG_CWD    0x02

#define HDD_CMD_SHOWID 0x02
#define HDD_CMD_DELID  0x03
#define HDD_CMD_READID 0x04
#define HDD_CMD_WRITID 0x05
#define HDD_CMD_MOVEID 0x06

#define HDD_CMD_REQID  0x08
#define HDD_CMD_REQNM  0x09
#define HDD_CMD_MKELEM 0x0a

#define HDD_CMD_SHOWNM 0x12
#define HDD_CMD_DELNM  0x13
#define HDD_CMD_READNM 0x14
#define HDD_CMD_WRITNM 0x15
#define HDD_CMD_MOVENM 0x16

#define HDD_CMD_RENAME 0x17

#define EXT_TXT 0x11
#define EXT_MSG 0x12
#define EXT_CGR 0x1A
#define EXT_GIF 0x1B
#define EXT_CPP 0xA1
#define EXT_SH  0xA2
#define EXT_ASM 0xA3
#define EXT_MP3 0xB1
#define EXT_SIG 0xB2
#define EXT_EXE 0xF1
#define EXT_TTF 0xF2
#define EXT_OBJ 0xF3
#define EXT_LNK 0x55
#define EXT_DIR 0xAA

class MassStorage : public Cx16Device
{
public:
    MassStorage(std::istream& disk_image, u16 _id_counter, u16 _filled_storage)
    : m_disk_image(disk_image), id_counter(_id_counter), filled_storage(_filled_storage) {}

    virtual u16 do_cmd(u8 cmd, const std::vector<u16>& args) override;
    virtual u8 get_argc(u8 cmd) const override;
    virtual u16* reg(u8 id) override;

    virtual u8 di_caps() const override { return 0x1; }

private:
    u16 list_by_id(u16 id);
    u16 delete_by_id(u16 id);
    u16 read_by_id(u16 id, u16 addr, u16 size);
    u16 write_by_id(u16 id, u16 addr, u16 size);
    u16 move_by_id(u16 target_folder_id, u16 file_id);

    u16 request_id(u16 name[4]);
    u16 make_element(u16 name[4], u8 ext, u16 folder_id);

    u16 HDD_CMD_SHOWNM 0x12
    u16 HDD_CMD_DELNM  0x13
    u16 HDD_CMD_READNM 0x14
    u16 HDD_CMD_WRITNM 0x15
    u16 HDD_CMD_MOVENM 0x16

    u16 HDD_CMD_RENAME 0x17

    u16 filled_storage = 0;
    u16 id_counter = 0;
    u16 user_address = 0;
};
