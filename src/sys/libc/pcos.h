#pragma once

#include <types.h>

#include <krnl/api/syscalls.h>

i16 bsearch(const char** set, u16 setsize, const char* value);
char consume(fd_t fd, char* buf, u16 bufsz, const char* delim);
i16 load_split_str(fd_t fd, char** bufs, u16 bufsz, u16 bufcount, char* delim);
errno_t spawn_wait(const char* exec, u16 argc, const char** argv);
//pid_t spawn(const char* exec, const char* arg1, const char* arg2);
