#pragma once

#include <types.h>

#define DI_STATUS_PRESENT      0x10
#define DI_STATUS_KNOWN_CAPS   0x20
#define DI_STATUS_EXT_CAPS     0x40
#define DI_STATUS_ASSIGNED     0x80
#define DI_STATUS_VERSION_MASK 0x0F

struct di_info
{
    u8 di_status;
    u8 di_caps;
    u16 di_extcaps;
};

void device_close(fd_t fd);
u16 device_cmd(fd_t fd, u8 cmd, u8 argc, u16 argt, ...);
void device_cmd_async(fd_t fd, u8 cmd, u8 argc, u16 argt, ...);
fd_t device_open(u8 id);
u16 device_read_reg(fd_t fd, u8 rid);
u16 device_write_reg(fd_t fd, u8 rid, u16 val);

u8 di_get_self(struct di_info* data);
u8 di_get_by_port(u8 port, struct di_info* data);
u8 di_get(fd_t fd, struct di_info* data);

void irq_handler(fd_t fd, void(*handler)(void));
void irqc_handler(fd_t fd, void(*handler)(u8, u16));
