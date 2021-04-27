## About 
CIMC is a 16-bit, loosely PC/x86 based redstone computer in Minecraft. The project
aims to be as scalable as possible.

## Architecture
The computer uses our own `cx16` architecture.

## Hardware features
* 16-bit CPU:
  * 4 general-purpose registers
  * Arithmetic operations
    * Add, subtract, multiply (wit 32-bit output!), divide, AND, OR, XOR, shift left, shift right
    * Reserved: Floating point !!
  * Stack
  * Simple x86 paging based virtual memory (64 KiB addressable memory, 512 B pages)
  * 16 I/O ports, 16-bit parallel bus
  * 8 external interrupts
  * CPU exceptions
  * 2 user accessible interrupts
  * 384B built-in BIOS ROM
  * CPU identification (CPUID)

* 7.625 KiB (7808 B) of RAM

* Chipset
  * 256 I/O ports
  * 16 IRQs
  * 16-bit parallel I/O bus
  * Special fast I/O bus ("Fast I/O")
  * Full DMA system

* Graphics adapter and display
  * Monitor (128x64, redstone lamp)
  * Text mode with ability to load custom font

* Mass storage (64 KiB)
  * 8 parallel 8 KiB-blocks "partitions" (effectively 7, one is for name registry)
  * Filesystem acceleration with directory structure
  * A maximum of 512 files (65535 IDs)
  * 12-character names (the last character is for extension)
  
* Sound adapter
  * 256 sound blocks for each speaker
  * A maximum of ?? parallel channels
  * 3-Speaker Dynamic Sound Distribution (DSD) for minimizing delays
  * 32B shift buffer for each channel
  
* (Almost) full keyboard (QWERTY)
  * Caps Lock, Alt, Control, Shift control keys
  
* Miscellaneous
  * Programmable Interval Timer (PIT)
  * Programmable interrupt controller (PIC), with interrupt masking
  * Real Time Clock - for querying MC time
  * Device Info (DI) interface used to detect and query device capabilities
  * Power Management Interface (PMI) for managing power state.
    * Support for Shutdown & Reboot.
    * Support for Power & Reset Buttons
    * Support for Power & HDD "LEDs"

* Additional features ;)
  * Full-pledged chassis
  * CPU and chassis fan
  * Network adapter

## Software features
* BIOS / Bootloader
  * Ability to load kernel from file (thanks to FS acceleration)
* Simple operating system (VERY loosely Unix-based)
  * Up to 3 processes (shell, command, worker) + kernel, not preemptive for now
  * File descriptors (up to ?? per process)
  * Standard streams
  * Simple shell with Unix-like commands
* Applications
  * Text editor
  * Calculator
  * Paint
  * Hex editor
  * Some game
  * CIMC demo, showing various features of the computer.

## Formats
See spec/OS/os-extensions.txt for details.

* Font, up to 256 characters (.ttf)
* Sound, up to 32B per channel (.mp3, .sig)
* Graphics, up to 16x16 (.cgr)
* Animation, up to 16x16, 8 frames (.gif)
* Executable - "normal" executable, library, object file (.exe, .obj)
* ASCII (.txt, .msg, .cpp, .sh, .asm)

## Development features
* NOTE: Currently all development software works only on Linux (Tested on Ubuntu 20.04)

* Full cx16 emulator -- WIP
* Compiler/toolchain (C-based) -- WIP
* Font generator (from image file) -- TODO
* Sound generator (from MIDI) -- TODO
* Full specification of entire architecture -- WIP

## Optimization ways
Because the worlds starts to lag, we must to find out a way a minimize it.

Possible ways are:
* Some no-lag plugin
* Custom MC server/MC client?

## Links
https://wiki.osdev.org/Main_Page - info about creating operating systems  
http://www.maverick-os.dk/FileSystemFormats/FAT12_FileSystem.html - FAT12 specification  
https://software.intel.com/content/dam/develop/public/us/en/documents/325462-sdm-vol-1-2abcd-3abcd.pdf - Full spec of x86  

