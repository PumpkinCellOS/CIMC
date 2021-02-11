mode freestanding
format raw
entry 0x0
size 512
output "rom.exe"
sections {
    .text
    .ivt
}
restrict .data, .bss
