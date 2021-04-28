#pragma once

#include <memory>

#include "ast.h"
#include "runtimedata.h"

namespace assembler
{

class InstructionOperation
{
public:
    InstructionOperation(const Instruction& instruction)
    : m_instruction(instruction) {}

    bool execute(RuntimeData&) const;

protected:
    const Instruction& m_instruction;
};

}
