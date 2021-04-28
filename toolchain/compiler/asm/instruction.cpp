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
    if(m_instruction.members.size() == 0)
    {
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
    }
    else if(m_instruction.members.size() == 1)
    {
        auto arg1 = m_instruction.members[0];
        if(mnemonic[0] == 'J')
        {
            std::string condition = mnemonic.substr(1);
            if(condition == "MP")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "Z" || condition == "E")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "G")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "L")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "NE" || condition == "NZ")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "GE")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else if(condition == "LE")
                opcode = std::make_shared<opcodes::Jump>(arg1, opcodes::Jump::None);
            else
                BUILDER_ERROR(&m_instruction, "invalid jump condition");
        }
        // TODO: PUSH, POP, NEG, NOT, CALL, LIVT, INT, INC, DEC, PGE, TRC
    }

    if(!opcode)
    {
        opcode = std::make_shared<opcodes::Invalid>();
        BUILDER_ERROR(&m_instruction, "invalid opcode");
    }

    data.current_section()->instructions.push_back(opcode);
    return true;
}

}
