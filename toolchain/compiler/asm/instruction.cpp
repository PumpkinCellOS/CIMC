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
    std::cout << "  Data instruction ::execute " << m_instruction.mnemonic << std::endl;
    return true;
}

bool Code::execute(RuntimeData& data) const
{
    std::cout << "  Code instruction ::execute " << m_instruction.mnemonic << std::endl;
    return true;
}

}

}
