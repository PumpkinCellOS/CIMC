// Allow only read.
class src8
{
    const u8 value;
    operator const u8() { return value; }
    trig bool allow_read();
};
class src16
{
    const u16 value;
    operator const u16() { return value; }
    trig bool allow_read();
};

// Allow only write.
class dst8
{
    u8 value;
    operator u8&() { return value; }
    trig bool allow_write();
};
class dst16
{
    u16 value;
    operator u16&() { return value; }
    trig bool allow_write();
};

class iaddr
{
    u16 value;
    operator u16&() { return value; }
    trig bool allow_jump_to();
};

class iostate
{
    bool error;
};

class flags
{
    u8 unused;
    bool rs1, rs2;
    bool IF, GF, OF, ZF, NF, PF;
    
    operator src16() { return { value = (u16)*this}; }
    trig void clear();
    trig void clear_arith();
};

enum isr_specifier
{
    #IM,
    #II,
    #UE,
    #PF,
    #R3,
    #R4,
    #U1,
    #U2,
    #IQ0,
    #IQ1,
    #IQ2,
    #IQ3,
    #IQ4,
    #IQ5,
    #IQ6,
    #IQ7,
};

// ---- MODULES ----
module io
{
    trig iostate output_data8(u4 port, src8 src);
    trig iostate output_data16(u4 port, src16 src);
    trig iostate input_data8(u4 port, dst8 dest);
    trig iostate input_data16(u4 port, dst16 dest);
};

module control
{
    trig void raise(isr_specifier spec);
    flags flags;
};

module instruction
{
    trig void jump_to(iaddr address);
};
