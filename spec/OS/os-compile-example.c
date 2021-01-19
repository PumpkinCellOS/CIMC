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

__attribute(libc) u16 exit(u16 rc);
/*{
    asm(
        "XOR ax, ax\n"  1
        "INT 0x6\n"     1
    ); // total size: 2B
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

void _start()
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
    mov bp, sp            ;1
    push _0               ;3
    call 0x060D ; strlen  ;3
    mov dx, ax            ;1
    pop ax                ;1
    mov bp, sp            ;1
    push8 0x1             ;2
    push ax               ;1
    push dx               ;1
    call 0x0600 ; write   ;3
    pop dx                ;1
    pop ax                ;1
    pop8 al               ;1
    mov bp, sp            ;1
    push16 0x0            ;3
    call 0x060B ; exit    ;3
                        ;27B
