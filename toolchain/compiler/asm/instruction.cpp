#include "instruction.h"

namespace assembler
{

bool InstructionOperation::execute(RuntimeData& data) const
{
    std::string mnemonic = m_instruction.mnemonic;

    std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(),
    [](char c){ return std::toupper(c); });

    std::shared_ptr<Opcode> opcode;

    // 0 arguments
    if(mnemonic == "II1")
        opcode = std::make_shared<opcodes::Invalid>();
    else if(mnemonic == "_RMSTFR")
        opcode = std::make_shared<opcodes::NoArgument>(0x89);
    else if(mnemonic == "PUSHA")
        opcode = std::make_shared<opcodes::NoArgument>(0x8c);
    else if(mnemonic == "POPA")
        opcode = std::make_shared<opcodes::NoArgument>(0x8e);
    else if(mnemonic == "RET")
        opcode = std::make_shared<opcodes::NoArgument>(0xba);
    else if(mnemonic == "IRET")
        opcode = std::make_shared<opcodes::NoArgument>(0xbb);
    else if(mnemonic == "HLT")
        opcode = std::make_shared<opcodes::NoArgument>(0xbc);
    else if(mnemonic == "_MKSTFR")
        opcode = std::make_shared<opcodes::NoArgument>(0xbd);
    else if(mnemonic == "CPUID")
        opcode = std::make_shared<opcodes::NoArgument>(0xbe);

    if(!opcode)
    {
        opcode = std::make_shared<opcodes::Invalid>();
        BUILDER_WARNING(&m_instruction, "invalid opcode");
    }

    data.current_section()->instructions.push_back(opcode);
    return true;
}

}
