.entry=0x0

section .stack:
    skip 128
section .ivt:
    skip 128
section .text

_start:
    ; Setup a memory layout.
    ; 0x0000-0x01FF (512B) BIOS data
    ;  0x0000-0x00FF (256B) Code, Rodata
    ;  0x0100-0x017F (128B) Stack
    ;  0x0180-0x01FF (128B) IVT
    
    ; Stack
    mov sp, .stack
    mov bp, .stack
    
    ; Jump to C code
    call bios_main
    
    ; We shouldn't go here - hang forever.
    _emu_dbg "Cheats??"
    _fdsbl IF
    hlt
