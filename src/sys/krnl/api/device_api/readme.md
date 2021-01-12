# Some info about device API

The device API - routines used as wrappers for devices. They correspond with device I/O convention (see reference).  
NOTE: These routines should NOT be used directly. They are used mainly by device drivers. Use libc routines instead.  

[Reference](/spec/Devices/devices.txt)  

## Call list

### device_open (0x00)
- *Synopsis*: `u8 device_open(u8 io);`
- *Purpose*: Opens a device descriptor that is used for further device operations.
- *Arguments*:
    * `id`: Device ID (corresponds to I/O port used by device)
- *Return value*: A device descriptor

### device_close (0x01)
- *Synopsis*: `void device_close(u8 fd);`
- *Purpose*: Frees up a device descriptor.
- *Arguments*:
    * `fd`: A device descriptor.

### device_cmd
- *Synopsis*: `u16 device_cmd(u8 fd, u8 cmd, u8 argc, u16 argt, ...);`
- *Purpose*: Performs a command on device descriptor.
- *Arguments*:
    * `fd`: A device descriptor.
    * `cmd`: A command to perform.
    * `argc`: \[0000yyyy\]  y-argument count.
    * `argt`: Argument type (0: 8-bit, 1: 16-bit)
    * `...`: Arguments for operation (count specified by `argc`).
- *Return value*: A result, or 0 if no result (see reference)

### device_cmd_async
- *Synopsis*: `void device_cmd_async(u8 fd, u8 cmd, u8 argc, u16 argt, ...);`
- *Purpose*: Perform an asynchronous command on device descriptor. Result is passed to function specified with `irqc_handler` syscall.
- *Arguments*:
    * `fd`: A device descriptor.
    * `cmd`: A command to perform.
    * `argc`: \[0000yyyy\]  y-argument count.
    * `argt`: Argument type (0: 8-bit, 1: 16-bit)
    * `...`: Arguments for operation (count specified by `argc`).
    
### device_read_reg
- *Synopsis*: `u16 device_read_reg(u8 fd, u8 rid);`
- *Purpose*: Reads a device register.
- *Arguments*:
    * `fd`: A device descriptor.
    * `rid`: Register ID (see reference)
- *Return value*: A register value.

### device_write_reg
- *Synopsis*: `u16 device_write_reg(u8 fd, u8 rid, u16 val);`
- *Purpose*: Writes a device register.
- *Arguments*:
    * `fd`: A device descriptor.
    * `rid`: Register ID (see reference)
    * `val`: Value.
- *Return value*: Status (See Error Codes)

### irq_handler
- *Synopsis*: `void irq_handler(u8 fd, void(*handler)(void));`
- *Purpose*: Adds an IRQ handler (ISR) for specified IRQ number.
- *Arguments*:
    * `fd`: A device descriptor.
    * `handler`: A function `void handler(void)` that gets called when IRQ is raised.

### irqc_handler
- *Synopsis*: `void irq_handler(u8 fd, void(*handler)(u8, u16));`
- *Purpose*: Adds an IRQ handler (ISR) for specified IRQ number.
- *Arguments*:
    * `fd`: A device descriptor.
    * `handler`: A function `void handler(u8 cmd, u16 result)` that gets called when IRQ is raised.
    
### di_get_self
- TODO
- *Purpose*: Loads Self Device Information. It's equivalent for di_get_by_port(0x9).

### di_get_by_port
- TODO
- *Purpose*: Loads Device Information for device conencted to specified `port`.
