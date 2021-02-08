// Example of code compiled for cx16 //

#include <cstdio>
#include <cstdlib>

void _start()
{
    const char* helloworld = "Hello world!";
    write(1, helloworld, strlen(helloworld));
    exit(0);
}

///////////////////////////////////////

__attribute(libc) u16 write(u8 fd, u16* buf, u16 bufsz);
/*{
    asm(
        "MOV ax, 0x2\n"     3
        "MOV bx, [bp+0]\n"  2
        "MOV cx, [bp+1]\n"  2   
        "MOV dx, [bp+3]\n"  2
        "INT #U1\n"         1
        "RET\n"             1
    ); // total size: 11B
}*/

__attribute(libc) [[noreturn]] u16 exit(u16 rc);
/*{
    asm(
        "XOR ax, ax\n"   1
        "MOV bx, [bp+0]" 2
        "INT #U1\n"      1
    ); // total size: 4B
}*/

__attribute(libc) u16 strlen(const char* str);
/*{
    u16 i = 0;
    for(;!str[i];i++) ;
    return i;
    asm(
        "XOR ax, ax\n"      1
        "strlen_L001: "
        "MOV bx, [bp+0]\n"  2
        "CMP bx, 0x0\n"     3
        "INC ax\n"          1
        "JNE strlen_L001\n" 3
        "RET\n"             1
    ); // total size: 11B
}*/

[[noreturn]] void _start()
{
    const char* helloworld = "Hello world!";
    write(1, helloworld, strlen(helloworld));
    exit(0);
}

///////////////////////////////////////

.section .rodata
_0 = "Hello world!" ;12B 

.section .text

.start:
    push _0               ;3
    calla 0x060F ; strlen ;3
    cpop   2              ;2
    push8 0x1             ;2
    push16 [bp+0]         ;2
    push16 ax             ;1
    calla 0x0600 ; write  ;3
    cpop 5                ;2
    push16 0x0            ;3
    calla 0x060B ; exit   ;3
                        ;24B
