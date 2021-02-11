stack=0x0180
_start=0x0

section .text

.globl bios_main
.type bios_main, @function

.globl _start
.type _start, @function
_start:
    ; Setup a memory layout.
    ; 0x0000-0x01FF (512B) BIOS data
    ;  0x0000-0x017F (384B) ROM
    ;   0x0000-0x015F (352B) Code (.text)
    ;   0x0160-0x017F (32B) IVT (.ivt)
    ;  0x0180-0x01FF (128B) RAM
    ;   0x0180-0x01FF (128B) Stack
    
    ; Stack
    mov sp, stack
    mov bp, stack
    
    ; Jump to C code
    jmp bios_main
    
    ; We shouldn't go here - hang forever.
    _emu_dbg "Cheats??"
    _fdsbl IF
    hlt

.globl hdd_entry
.type hdd_entry, @function

section .ivt
    data 0x0B
    skip 22
    data &hdd_entry
