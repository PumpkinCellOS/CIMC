#include "HDD.h"

u16 MassStorage::do_cmd(u8 cmd, const std::vector<u16>& args)
{
    u16 name[4];
    switch(cmd)
    {
        case HDD_CMD_SHOWID: return list_by_id(args[0]);
        case HDD_CMD_DELID: return delete_by_id(args[0]);
        case HDD_CMD_READID: return read_by_id(args[0], args[1], args[2]);
        case HDD_CMD_WRITID: return write_by_id(args[0], args[1], args[2]);
        case HDD_CMD_MOVEID: return move_by_id(args[0], args[1]);

        case HDD_CMD_REQID:
            for(size_t s = 0; s < 4; s++) name[s] = args[s];
            return request_id(name);
        case HDD_CMD_MKELEM:
            for(size_t s = 0; s < 4; s++) name[s] = args[s];
            return make_element(name, args[4], args[5]);

        case HDD_CMD_SHOWNM:
            for(size_t s = 0; s < 4; s++) name[s] = args[s];
            return list_by_name(name);
        case HDD_CMD_DELNM:
            for(size_t s = 0; s < 4; s++) name[s] = args[s];
            return delete_by_name(name);
        case HDD_CMD_READNM:
            for(size_t s = 0; s < 4; s++) name[s] = args[s];
            return read_by_name(name, args[4], args[5]);
        case HDD_CMD_WRITNM:
            for(size_t s = 0; s < 4; s++) name[s] = args[s];
            return write_by_name(name, args[4], args[5]);
        case HDD_CMD_MOVENM:
            for(size_t s = 0; s < 4; s++) name[s] = args[s + 1];
            return move_by_name(args[0], name);
        case HDD_CMD_RENAME:
            for(size_t s = 0; s < 4; s++) name[s] = args[s + 1];
            return rename(args[0], name);
        default:
            return 0;
    }
}

u8 MassStorage::get_argc(u8 cmd) const
{
    switch(cmd)
    {
        case HDD_CMD_SHOWID: return 1;
        case HDD_CMD_DELID: return 1;
        case HDD_CMD_READID: return 3;
        case HDD_CMD_WRITID: return 3;
        case HDD_CMD_MOVEID: return 2;

        case HDD_CMD_REQID: return 4;
        case HDD_CMD_MKELEM: return 6;

        case HDD_CMD_SHOWNM: return 4;
        case HDD_CMD_DELNM: return 4;
        case HDD_CMD_READNM: return 6;
        case HDD_CMD_WRITNM: return 6;
        case HDD_CMD_MOVENM: return 5;
        case HDD_CMD_RENAME: return 5;
        default: return Cx16ConventionalDevice::get_argc(cmd);
    }
}

u16* MassStorage::reg(u8 id)
{
    switch(id)
    {
        case HDD_REG_FILLED: return &filled_storage;
        case HDD_REG_IDC: return &id_counter;
        case HDD_REG_CWD: return &user_address;
        default: return nullptr;
    }
}

u16 MassStorage::list_by_id(u16 id)
{
    return 0;
}
u16 MassStorage::delete_by_id(u16 id)
{
    return 0;
}
u16 MassStorage::read_by_id(u16 id, u16 addr, u16 size)
{
    return 0;
}
u16 MassStorage::write_by_id(u16 id, u16 addr, u16 size)
{
    return 0;
}
u16 MassStorage::move_by_id(u16 target_folder_id, u16 file_id)
{
    return 0;
}

u16 MassStorage::request_id(u16 name[4])
{
    return 0;
}

u16 MassStorage::make_element(u16 name[4], u8 ext, u16 folder_id)
{
    return 0;
}

u16 MassStorage::list_by_name(u16 name[4])
{
    return 0;
}
u16 MassStorage::delete_by_name(u16 name[4])
{
    return 0;
}
u16 MassStorage::read_by_name(u16 name[4], u16 addr, u16 size)
{
    return 0;
}
u16 MassStorage::write_by_name(u16 name[4], u16 addr, u16 size)
{
    return 0;
}
u16 MassStorage::move_by_name(u16 target_folder_id, u16 name[4])
{
    return 0;
}

u16 MassStorage::rename(u16 id, u16 new_name[4])
{
    return 0;
}
