typedef unsigned char u8;
typedef unsigned int u16;

#define HALT asm("hlt\n")

#define reg(x) __attribute((__reg(x)))

inline void out8(const reg(imm) u8 port, const reg(al|imm) u8 data)
{
    asm(
        "out8 port, data\n"
    : "imm"(port), ""(data)
    );
}

inline void out16(const reg(imm) u8 port, const reg(ax|imm) u16 data)
{
    asm(
        "out16 port, data\n"
    : "imm"(port), ""(data)
    );
}

inline __attribute((noreturn)) u8 in8(const reg(imm) u8 port)
{
    asm(
        "in8 port, al\n"
        "ret\n"
    : "imm"(port)
    );
}

inline __attribute((noreturn)) u16 in16(const reg(imm) u8 port)
{
    asm(
        "in16 port, ax\n"
        "ret\n"
    : "imm"(port)
    );
}

#define HDD_IO 0x02
#define HDD_CMD_GET_ID 0x08
#define HDD_CMD_READ_BY_NAME 0x14

asm(
    "hdd_entry:\n"
        "in8 0x02, ax\n" // To not hang the HDD
        "mov dx, 0x0\n" // Note that we cannot use global variables here, so we use 'dx'.
        "iret\n"
);

void setup_ivt()
{
    // Load interrupt table.
    asm(
        "livt 0x0160\n"
        // TODO: Consider masking other devices' IRQ lines.
    );
}

u16 load_os()
{
    // Find a file on disk called "/sys/krnl".
    
    // Goto /sys (it has a hardcoded ID 0x0001).
    out16(HDD_IO, 0x0102); //Write Register: User Address
    out16(HDD_IO, 0x0001);
    
    // Read /krnl.
    out8(HDD_IO, HDD_CMD_READ_BY_NAME);
    out16(HDD_IO, 0x6b72); // "kr"
    out16(HDD_IO, 0x6e6c); // "nl"
    out16(HDD_IO, 0x0000); // padding
    out16(HDD_IO, 0x0000);
    out16(HDD_IO, 0x0400); // address
    out16(HDD_IO, 0x0000); // offset
    out16(HDD_IO, 0x0200); // size
    
    // Wait for IRQ. The IRQ handler sets DX to 1.
    asm(
        "_fenbl IF\n" // Flag Enable: Interrupt Flag
        "mov dx, 0x0\n"
        "1:\n"
            "hlt\n"
            // Got an IRQ.
            "cmp dx, 0x0\n"
            // If jumped, the IRQ was unrelated to HDD.
            // TODO: Crash when other IRQs will be masked and only CPU exceptions / NMIs will be allowed.
            "je 1\n"
        "_fdsbl IF\n" //Disable IRQs.
    );
    
    u16* os = (u16*)0x0400;
    if(os[0] != 0x5080) {
        __cx16_emulator_debug("Invalid boot image");
        HALT;
    }
    
    // Find an entry point of OS.
    return os[1];
}

__attribute((noreturn)) void bios_main()
{
    setup_ivt();
    u16 os_addr = load_os();
    
    // Initialize Network Interface Controller
    __cx16_do("init(nic)")
    __cx16_info("NIC initialized");
    
    // Initialize CPU Fan
    __cx16_do("init(cpu-fan)")
    __cx16_info("CPU Fan initialized");
    
    // Give control to OS. The OS is responsible
    // for loading a new IVT, setup paging etc.
    // Note that the OS will get back the BIOS IVT
    // if it enables interrupts without setting up
    // it's own IVT.
    asm(
        "jmp ax"
    : "ax"(os_addr)
    );
}
