#include "instruction.h"

namespace assembler
{

std::shared_ptr<InstructionOperation> InstructionOperation::create(const Instruction& instruction, const RuntimeData::Section* section)
{
    // TODO: Allow specify section type (data/code)
    if(!section || section->name == "text") // no section, .text
    {
        return std::make_shared<instructions::Code>(instruction);
    }
    else // .data, .rodata, .bss and any other
    {
        return std::make_shared<instructions::Data>(instruction);
    }
}

namespace instructions
{

bool Data::execute(RuntimeData& data) const
{
    std::string mnemonic = m_instruction.mnemonic;

    std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(),
    [](char c){ return std::toupper(c); });

    std::shared_ptr<Opcode> opcode;

    if(mnemonic == "II1")
        opcode = std::make_shared<opcodes::InvalidOpcode>();

    if(!opcode)
    {
        opcode = std::make_shared<opcodes::InvalidOpcode>();
        BUILDER_WARNING(&m_instruction, "invalid opcode");
    }

    data.current_section()->instructions.push_back(opcode);
    return true;
}

bool Code::execute(RuntimeData& data) const
{
    std::string mnemonic = m_instruction.mnemonic;

    std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(),
    [](char c){ return std::toupper(c); });

    std::shared_ptr<Opcode> opcode;

    if(mnemonic == "II1")
        opcode = std::make_shared<opcodes::InvalidOpcode>();
    else if(mnemonic == "_RMSTFR")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0x89);
    else if(mnemonic == "PUSHA")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0x8c);
    else if(mnemonic == "POPA")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0x8e);
    else if(mnemonic == "RET")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0xba);
    else if(mnemonic == "IRET")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0xbb);
    else if(mnemonic == "HLT")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0xbc);
    else if(mnemonic == "_MKSTFR")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0xbd);
    else if(mnemonic == "CPUID")
        opcode = std::make_shared<opcodes::NoArgumentOpcode>(0xbe);

    if(!opcode)
    {
        opcode = std::make_shared<opcodes::InvalidOpcode>();
        BUILDER_WARNING(&m_instruction, "invalid opcode");
    }

    data.current_section()->instructions.push_back(opcode);
    return true;
}

}

}
