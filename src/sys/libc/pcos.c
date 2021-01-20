#include <pcos.h>

#include <errno.h>
#include <stdlib.h>

i16 bsearch(const char** set, u16 setsize, const char* value)
{
    return -1;
}

char consume(fd_t fd, char* buf, u16 bufsz, const char* delim)
{
    u16 off = 0;
    while(1) {
        if(off >= bufsz)
            return 0;
        if(read(fd, buf + off, 1) < 0)
            return -1;
        for(char* _cd = (char*)delim; *_cd; _cd++) {
            if(buf[off] == *_cd)
                return *_cd;
        }
        off++;
    }
}

// delim = "SD" s-split character (' ') d-delimiter ('\n')
// returns split count, -1 too many splits, -2 too big splits, -3 system error
i16 load_split_str(fd_t fd, char** bufs, u16 bufsz, u16 bufcount, char* delim)
{
    for(u16 i = 0; i < bufcount; i++)
    {
        rc = consume(fd, bufs[i], 8, delim);
        if(rc < 0) {
            return -3;
        }
        else if(rc == 0) {
            return -2;
        }
        else if(rc == delim[0]) {
            continue;
        }
        else { // Newline
            return i;
        }
    }
    return -1;
}

u16 split_str(const char* in, char** bufs, u16 bufcount)
{
    // TODO
    return 0;
}

errno_t spawn_wait(const char* exec, u16 argc, const char* argv)
{
    //__builtin_syscall(
    return ENOSYS;
}
