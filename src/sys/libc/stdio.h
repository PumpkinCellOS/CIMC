#pragma once

#include <types.h>

#define FAPPEND 0x01
#define FCREATE 0x02
#define FDIR    0x04
#define FREAD   0x08
#define FTOUCH  0x10
#define FWRITE  0x20

#define IO_BLOCK 0x01

#define STDIN  0
#define STDOUT 1
#define STDERR 2

errno_t chdir(char* path);
errno_t close(fd_t fd);
errno_t gets(const char* str, u16 count);
errno_t ioctl(fd_t fd, u16 op, u16 arg);
errno_t mkdir(char* path);
fd_t open(char* path, u8 mode);
errno_t perror(char* prefix);
errno_t puts(char* str);
errno_t read(fd_t fd, char* buf, u16 count);
errno_t write(fd_t fd, const char* buf, u16 count);
