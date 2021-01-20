#include <pcos.h>
#include <stdio.h>
#include <stdlib.h>

#define PROMPT "> "

const char* BUILTINS[] = {
    "cd",
    "exit",
    "help",
    "mkdir",
    "touch"
};

int main(u16 argc, char* argv[])
{
    while(1) {
        puts(PROMPT);
        char argv[3][8];
        
        // i16 load_split_str(fd_t fd, const char** bufs, u16 bufsz, u16 bufcount, char* delim)
        // returns split count, -1 too many splits, -2 too big splits, -3 system error
        i16 argc = load_split_str(STDIN, (char**)argv, 8, 3, " \n");
        switch(argc) {
            case -1:
                puts("too many args\n");
                break;
            case -2:
                puts("arg/cmd too long\n");
                break;
            case -3:
                perror("system error\n");
                break;
            case 0: continue;
        }
        
        switch(bsearch(BUILTINS, sizeof(BUILTINS), argv[0])) {
            case 0:
                if(argc < 1)
                    break;
                chdir(argv[1]);
                break;
            case 1:
                return 0;
            case 2:
                puts("cd, exit, help\n");
                break;
            case 3:
                if(argc < 1)
                    break;
                open(argv[1], FCREATE | FTOUCH | FDIR);
                break;
            case 4:
                if(argc < 1)
                    break;
                open(argv[1], FCREATE | FTOUCH);
                break;
            default:
                if(spawn_wait(argv[0], argc, (const char**)(argv + 1)) < 0)
                    perror("spawn");
                break;
        }
    }
    return 0;
}
