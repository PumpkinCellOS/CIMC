#pragma once

class ControlUnit;

class Executor
{
public:
    Executor(ControlUnit& control)
    : m_control(control) {}

    // Instructions CANNOT see registers directly (they don't know their operands).
    void _INSN_OUT(bool width, u8 port, const Source& src);
    void _INSN_IN(bool width, u8 port, Destination& dst);
    void _INSN_MOV(Destination& dst, const Source& src);
    void _INSN_JMP(const Source& ia);
    void _INSN_JZ(const Source& ioff);
    void _INSN_JG(const Source& ioff);
    void _INSN_JL(const Source& ioff);
    void _INSN_JNE(const Source& ioff);
    void _INSN_JGE(const Source& ioff);
    void _INSN_JLE(const Source& ioff);
    void _INSN_CMP(const Source& op1, const Source& op2);
    void _INSN_PUSH(bool width, const Source& value);
    void _INSN_POP(bool width, Destination& value);
    // TODO: PUSHA, POPA
    // TODO: Add, Subtract, Multiply/Divice, And, Or, Other Byte, Call
    void _INSN_ADD(Data& dst, const Source& src);
    void _INSN_SUB(Data& dst, const Source& src);
    void _INSN_LIVT(const Source& addr);
    void _INSN_INT(u8 int_number);
    // TODO: Random Block 1
    void _INSN_HLT();
    // TODO: _MKSTFR
    void _INSN_CPUID();

private:
    void jmp_helper(const Source& ioff);

    ControlUnit& m_control;
};
